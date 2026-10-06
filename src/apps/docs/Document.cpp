#include "Document.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QDateTime>

namespace blastmaster::docs {

Document::Document()
    : m_title("Untitled Document")
    , m_content("")
    , m_filePath("")
    , m_isDirty(false)
{
}

Document::Document(const QString& filePath)
    : m_title("Untitled Document")
    , m_content("")
    , m_filePath(filePath)
    , m_isDirty(false)
{
}

QJsonObject Document::toJson() const
{
    QJsonObject root;
    root["version"] = "1.0";
    root["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["title"] = m_title;
    root["content"] = m_content;
    return root;
}

void Document::fromJson(const QJsonObject& json)
{
    if (json.contains("title")) {
        m_title = json["title"].toString();
    }
    if (json.contains("content")) {
        m_content = json["content"].toString();
    }
}

bool Document::save()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Document path is empty";
        return false;
    }
    return saveAs(m_filePath);
}

bool Document::saveAs(const QString& filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "Cannot save to empty path";
        return false;
    }

    QString path = filePath;
    if (!path.endsWith(".dccx")) {
        path += ".dccx";
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    QJsonDocument doc(toJson());
    QByteArray jsonData = doc.toJson();

    if (file.write(jsonData) == -1) {
        qWarning() << "Failed to write document data";
        file.close();
        return false;
    }

    file.close();
    m_filePath = path;
    m_isDirty = false;
    qDebug() << "Document saved to" << path;
    return true;
}

bool Document::load()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Document path is empty";
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << m_filePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning() << "Invalid .dccx document format";
        return false;
    }

    fromJson(doc.object());
    m_isDirty = false;
    qDebug() << "Document loaded from" << m_filePath;
    return true;
}

} // namespace blastmaster::docs
