#pragma once

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QStringList>

namespace blastmaster::database {

class DatabaseDocument : public QObject {
    Q_OBJECT
public:
    explicit DatabaseDocument(QObject* parent = nullptr);
    ~DatabaseDocument() override;

    bool newDatabase();
    bool openDatabase(const QString& path);
    bool saveDatabase();
    bool saveDatabaseAs(const QString& path);
    bool closeDatabase();

    bool isOpen() const { return m_db.isValid() && m_db.isOpen(); }
    QString filePath() const { return m_filePath; }
    QString lastError() const { return m_lastError; }

    QStringList tables() const;
    bool createTable(const QString& name);
    bool deleteTable(const QString& name);

signals:
    void databaseChanged();
    void errorOccurred(const QString& message);

private:
    bool ensureConnection();
    void setError(const QString& error);

    QSqlDatabase m_db;
    QString m_connectionName;
    QString m_filePath;
    QString m_lastError;
};

} // namespace blastmaster::database
