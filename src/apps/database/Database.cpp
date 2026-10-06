#include "Database.h"

#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>

namespace blastmaster::database {

Database::Database()
    : m_title(QStringLiteral("Database1"))
    , m_isDirty(false)
    , m_connectionName(QStringLiteral("blastmaster_database_%1")
          .arg(reinterpret_cast<quintptr>(this)))
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
}

Database::Database(const QString& filePath)
    : Database()
{
    openPath(filePath);
}

Database::~Database()
{
    if (m_db.isValid())
        m_db.close();

    const QString name = m_connectionName;
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(name);
}

bool Database::openPath(const QString& path)
{
    if (path.isEmpty())
        return false;

    if (m_db.isOpen())
        m_db.close();

    m_db.setDatabaseName(path);
    if (!m_db.open())
        return false;

    m_filePath = path;
    m_title = QFileInfo(path).completeBaseName();
    m_isDirty = false;
    return true;
}

bool Database::load()
{
    return openPath(m_filePath);
}

bool Database::save()
{
    if (m_filePath.isEmpty())
        return false;

    // SQLite writes changes directly to the database file.
    m_isDirty = false;
    return isOpen();
}

bool Database::saveAs(const QString& filePath)
{
    if (filePath.isEmpty())
        return false;

    if (m_db.isOpen())
        m_db.close();

    m_db.setDatabaseName(filePath);
    if (!m_db.open())
        return false;

    m_filePath = filePath;
    m_title = QFileInfo(filePath).completeBaseName();
    m_isDirty = false;
    return true;
}

QStringList Database::tables() const
{
    if (!isOpen())
        return {};
    return m_db.tables(QSql::Tables);
}

bool Database::createTable(const QString& name)
{
    if (!isOpen() || name.trimmed().isEmpty())
        return false;

    const QString safeName = name.trimmed().replace('"', QStringLiteral(""""));
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS "%1" "
            "(ID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Value TEXT)")
            .arg(safeName)))
        return false;

    m_isDirty = true;
    return true;
}

bool Database::deleteTable(const QString& name)
{
    if (!isOpen() || name.trimmed().isEmpty())
        return false;

    const QString safeName = name.trimmed().replace('"', QStringLiteral(""""));
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("DROP TABLE IF EXISTS "%1"").arg(safeName)))
        return false;

    m_isDirty = true;
    return true;
}

} // namespace blastmaster::database
