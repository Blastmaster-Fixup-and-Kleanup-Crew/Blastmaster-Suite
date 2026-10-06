#pragma once

#include <QString>
#include <QJsonObject>

namespace blastmaster::spreadsheet {

class Workbook {
public:
    Workbook();
    explicit Workbook(const QString& filePath);

    QString title() const { return m_title; }
    void setTitle(const QString& t) { m_title = t; m_isDirty = true; }

    QString filePath() const { return m_filePath; }
    void setFilePath(const QString& p) { m_filePath = p; }

    bool isDirty() const { return m_isDirty; }
    void setClean() const { m_isDirty = false; }

    // File operations
    bool save();
    bool saveAs(const QString& filePath);
    bool load();

    // File metadata helpers
    static QString fileExtension() { return ".bmsx"; }
    static QString fileFilter() { return "Blastmaster Workbooks (*.bmsx);;All Files (*.*)"; }

private:
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

    QString m_title;
    QString m_filePath;
    bool m_isDirty;
};

} // namespace blastmaster::spreadsheet
