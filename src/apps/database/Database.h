#pragma once

#include <QString>
#include <QStringList>
#include <QSqlDatabase>

namespace blastmaster::database {

/**
 * @class Database
 * @brief Represents a Blastmaster Database document backed by SQLite.
 *
 * The class follows the same document-model layout used by Workbook,
 * Document, and PresentationDocument in the other Blastmaster Suite apps.
 */
class Database {
public:
    Database();
    explicit Database(const QString& filePath);
    ~Database();

    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; m_isDirty = true; }

    QString filePath() const { return m_filePath; }
    void setFilePath(const QString& path) { m_filePath = path; }

    bool isDirty() const { return m_isDirty; }
    void setClean() { m_isDirty = false; }

    bool isOpen() const { return m_db.isValid() && m_db.isOpen(); }

    bool save();
    bool saveAs(const QString& filePath);
    bool load();

    QStringList tables() const;
    bool createTable(const QString& name);
    bool deleteTable(const QString& name);

    static QString fileExtension() { return ".dbx"; }
    static QString fileFilter() { return "Blastmaster Databases (*.dbx *.db);;All Files (*.*)"; }
    static QString defaultSuffix() { return "dbx"; }

private:
    bool openPath(const QString& path);
    QString connectionName() const { return m_connectionName; }

    QString m_title;
    QString m_filePath;
    bool m_isDirty;
    QString m_connectionName;
    QSqlDatabase m_db;
};

} // namespace blastmaster::database
