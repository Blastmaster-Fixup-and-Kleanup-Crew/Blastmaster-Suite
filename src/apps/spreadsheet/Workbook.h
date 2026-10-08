#pragma once

#include <QHash>
#include <QJsonObject>
#include <QString>
#include <QStringList>

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

    // Cell data is stored as sheet name -> cell address -> text/formula.
    QString cell(const QString& sheet, const QString& address) const;
    void setCell(const QString& sheet, const QString& address, const QString& value);
    QStringList sheets() const;
    void ensureSheet(const QString& sheet);
    bool addSheet(const QString& sheet);
    bool removeSheet(const QString& sheet);
    QString evaluateCell(const QString& sheet, const QString& address) const;

    bool save();
    bool saveAs(const QString& filePath);
    bool load();

    static QString fileExtension() { return ".wkbx"; }
    static QString fileFilter() { return "Blastmaster Workbooks (*.wkbx);;All Files (*.*)"; }

private:
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

    QString m_title;
    QString m_filePath;
    bool m_isDirty;
    QHash<QString, QHash<QString, QString>> m_cells;
};

} // namespace blastmaster::spreadsheet
