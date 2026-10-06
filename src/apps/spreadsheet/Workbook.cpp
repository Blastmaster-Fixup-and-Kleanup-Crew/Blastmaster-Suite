#include "Workbook.h"

#include <QFile>
#include <QJsonDocument>
#include <QDateTime>
#include <QDebug>

namespace blastmaster::spreadsheet {

Workbook::Workbook()
    : m_title("Book1")
    , m_filePath()
    , m_isDirty(false)
{
}

Workbook::Workbook(const QString& filePath)
    : m_title("Book1")
    , m_filePath(filePath)
    , m_isDirty(false)
{
}

QJsonObject Workbook::toJson() const
{
    QJsonObject root;
    root["version"] = "1.0";
    root["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["title"] = m_title;
    // reserve place for sheet metadata, cells, etc.
    root["sheets"] = QJsonObject();
    return root;
}

void Workbook::fromJson(const QJsonObject& json)
{
    if (json.contains("title")) m_title = json["title"].toString();
    // sheet/content parsing can be added later
}

bool Workbook::save()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Workbook::save - empty file path";
        return false;
    }
    return saveAs(m_filePath);
}

bool Workbook::saveAs(const QString& filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "Workbook::saveAs - empty path";
        return false;
    }

    QString path = filePath;
    if (!path.endsWith(Workbook::fileExtension())) path += Workbook::fileExtension();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    QJsonDocument doc(toJson());
    QByteArray bytes = doc.toJson();

    if (file.write(bytes) == -1) {
        qWarning() << "Failed to write workbook data";
        file.close();
        return false;
    }

    file.close();
    m_filePath = path;
    m_isDirty = false;
    qDebug() << "Workbook saved to" << path;
    return true;
}

bool Workbook::load()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Workbook::load - empty path";
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << m_filePath;
        return false;
    }

    QByteArray bytes = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject()) {
        qWarning() << "Invalid workbook format";
        return false;
    }

    fromJson(doc.object());
    m_isDirty = false;
    qDebug() << "Workbook loaded from" << m_filePath;
    return true;
}

} // namespace blastmaster::spreadsheet
