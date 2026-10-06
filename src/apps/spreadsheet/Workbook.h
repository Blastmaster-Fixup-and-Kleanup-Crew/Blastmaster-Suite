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
    void setClean() { m_isDirty = false; }

    // File operations
    bool save();
    bool saveAs(const QString& filePath);
    bool load();

    // File metadata helpers
    static QString fileExtension() { return ".wkbx"; }
    static QString fileFilter() { return "Blastmaster Workbooks (*.wkbx);;All Files (*.*)"; }

private:
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

    QString m_title;
    QString m_filePath;
    bool m_isDirty;
};

} // namespace blastmaster::spreadsheet
