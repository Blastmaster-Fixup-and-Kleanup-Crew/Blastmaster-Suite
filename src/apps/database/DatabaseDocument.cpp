#include "DatabaseDocument.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

namespace blastmaster::database {

DatabaseDocument::DatabaseDocument(QObject* parent)
    : QObject(parent)
    , m_connectionName(QStringLiteral("blastmaster_database_%1").arg(reinterpret_cast<quintptr>(this)))
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
}

DatabaseDocument::~DatabaseDocument()
{
    if (m_db.isValid()) m_db.close();
    const QString name = m_connectionName;
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(name);
}

bool DatabaseDocument::ensureConnection()
{
    if (!m_db.isValid()) {
        setError(QStringLiteral("SQLite database connection is unavailable."));
        return false;
    }
    return true;
}

void DatabaseDocument::setError(const QString& error)
{
    m_lastError = error;
    emit errorOccurred(error);
}

bool DatabaseDocument::newDatabase()
{
    if (!ensureConnection()) return false;
    m_db.close();
    m_filePath.clear();
    m_db.setDatabaseName(QStringLiteral(":memory:"));
    if (!m_db.open()) { setError(m_db.lastError().text()); return false; }

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS SampleData (ID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Value TEXT)"))) {
        setError(query.lastError().text());
        return false;
    }
    emit databaseChanged();
    return true;
}

bool DatabaseDocument::openDatabase(const QString& path)
{
    if (!ensureConnection() || path.isEmpty()) return false;
    m_db.close();
    m_db.setDatabaseName(path);
    if (!m_db.open()) { setError(m_db.lastError().text()); return false; }
    m_filePath = path;
    emit databaseChanged();
    return true;
}

bool DatabaseDocument::saveDatabase()
{
    if (m_filePath.isEmpty()) {
        setError(QStringLiteral("This database has not been saved to a file yet."));
        return false;
    }
    return saveDatabaseAs(m_filePath);
}

bool DatabaseDocument::saveDatabaseAs(const QString& path)
{
    if (!ensureConnection() || path.isEmpty()) return false;
    const QString source = m_db.databaseName();
    if (m_db.isOpen()) m_db.close();

    if (!source.isEmpty() && source != QStringLiteral(":memory:") && QFile::exists(source) && source != path) {
        if (QFile::exists(path) && !QFile::remove(path)) {
            setError(QStringLiteral("Could not replace the existing database file."));
            return false;
        }
        if (!QFile::copy(source, path)) {
            setError(QStringLiteral("Could not save the database file."));
            return false;
        }
    }
    m_db.setDatabaseName(path);
    if (!m_db.open()) { setError(m_db.lastError().text()); return false; }
    m_filePath = path;
    emit databaseChanged();
    return true;
}

bool DatabaseDocument::closeDatabase()
{
    if (!ensureConnection()) return false;
    m_db.close();
    m_filePath.clear();
    emit databaseChanged();
    return true;
}

QStringList DatabaseDocument::tables() const
{
    if (!m_db.isValid() || !m_db.isOpen()) return {};
    return m_db.tables(QSql::Tables);
}

bool DatabaseDocument::createTable(const QString& name)
{
    if (!m_db.isOpen() || name.trimmed().isEmpty()) return false;
    const QString safe = name.trimmed().replace('"', QStringLiteral("\"\""));
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS \"%1\" (ID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Value TEXT)").arg(safe))) {
        setError(query.lastError().text());
        return false;
    }
    emit databaseChanged();
    return true;
}

bool DatabaseDocument::deleteTable(const QString& name)
{
    if (!m_db.isOpen() || name.trimmed().isEmpty()) return false;
    const QString safe = name.trimmed().replace('"', QStringLiteral("\"\""));
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("DROP TABLE IF EXISTS \"%1\"").arg(safe))) {
        setError(query.lastError().text());
        return false;
    }
    emit databaseChanged();
    return true;
}

QStringList DatabaseDocument::columns(const QString& table) const
{
    if (!m_db.isOpen() || table.trimmed().isEmpty()) return {};
    const QString safe = table.trimmed().replace('"', QStringLiteral("\"\""));
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("PRAGMA table_info(\"%1\")").arg(safe))) return {};
    QStringList result;
    while (query.next()) result << query.value(1).toString();
    return result;
}

QList<QVariantList> DatabaseDocument::records(const QString& table, int limit) const
{
    QList<QVariantList> result;
    if (!m_db.isOpen() || table.trimmed().isEmpty()) return result;
    const QString safe = table.trimmed().replace('"', QStringLiteral("\"\""));
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("SELECT * FROM \"%1\" LIMIT %2").arg(safe).arg(qMax(1, limit)))) return result;
    while (query.next()) {
        QVariantList row;
        for (int i = 0; i < query.record().count(); ++i) row << query.value(i);
        result << row;
    }
    return result;
}

bool DatabaseDocument::insertRecord(const QString& table, const QVariantMap& values)
{
    if (!m_db.isOpen() || values.isEmpty()) return false;
    const QString safe = table.trimmed().replace('"', QStringLiteral("\"\""));
    QStringList fields, placeholders;
    QVariantList bindValues;
    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        fields << QStringLiteral("\"%1\"").arg(it.key().replace('"', QStringLiteral("\"\"")));
        placeholders << QStringLiteral("?");
        bindValues << it.value();
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("INSERT INTO \"%1\" (%2) VALUES (%3)").arg(safe, fields.join(", "), placeholders.join(", ")))) {
        setError(query.lastError().text()); return false;
    }
    for (const auto& value : bindValues) query.addBindValue(value);
    if (!query.exec()) { setError(query.lastError().text()); return false; }
    emit databaseChanged();
    return true;
}

bool DatabaseDocument::updateRecord(const QString& table, int rowId, const QVariantMap& values)
{
    if (!m_db.isOpen() || rowId < 0 || values.isEmpty()) return false;
    const QString safe = table.trimmed().replace('"', QStringLiteral("\"\""));
    QStringList assignments;
    QVariantList bindValues;
    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        assignments << QStringLiteral("\"%1\" = ?").arg(it.key().replace('"', QStringLiteral("\"\"")));
        bindValues << it.value();
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("UPDATE \"%1\" SET %2 WHERE ID = ?").arg(safe, assignments.join(", ")))) {
        setError(query.lastError().text()); return false;
    }
    for (const auto& value : bindValues) query.addBindValue(value);
    query.addBindValue(rowId);
    if (!query.exec()) { setError(query.lastError().text()); return false; }
    emit databaseChanged();
    return true;
}

bool DatabaseDocument::deleteRecord(const QString& table, int rowId)
{
    if (!m_db.isOpen() || rowId < 0) return false;
    const QString safe = table.trimmed().replace('"', QStringLiteral("\"\""));
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("DELETE FROM \"%1\" WHERE ID = ?").arg(safe))) {
        setError(query.lastError().text()); return false;
    }
    query.addBindValue(rowId);
    if (!query.exec()) { setError(query.lastError().text()); return false; }
    emit databaseChanged();
    return true;
}

} // namespace blastmaster::database
