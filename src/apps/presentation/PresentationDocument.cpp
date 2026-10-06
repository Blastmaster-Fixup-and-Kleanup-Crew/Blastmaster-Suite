#include "PresentationDocument.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace blastmaster::presentation {

PresentationDocument::PresentationDocument()
    : m_title("Untitled Presentation")
    , m_content("")
    , m_filePath("")
    , m_isDirty(false)
{
}

PresentationDocument::PresentationDocument(const QString& filePath)
    : m_title("Untitled Presentation")
    , m_content("")
    , m_filePath(filePath)
    , m_isDirty(false)
{
}

QJsonObject PresentationDocument::toJson() const
{
    QJsonObject root;
    root["version"] = "1.0";
    root["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["title"] = m_title;
    root["content"] = m_content;
    return root;
}

void PresentationDocument::fromJson(const QJsonObject& json)
{
    if (json.contains("title")) {
        m_title = json["title"].toString();
    }
    if (json.contains("content")) {
        m_content = json["content"].toString();
    }
}

bool PresentationDocument::save()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Presentation path is empty";
        return false;
    }
    return saveAs(m_filePath);
}

bool PresentationDocument::saveAs(const QString& filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "Cannot save to empty path";
        return false;
    }

    QString path = filePath;
    if (!path.endsWith(".prex")) {
        path += ".prex";
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    QJsonDocument doc(toJson());
    QByteArray jsonData = doc.toJson();
    if (file.write(jsonData) == -1) {
        qWarning() << "Failed to write presentation data";
        file.close();
        return false;
    }

    file.close();
    m_filePath = path;
    m_isDirty = false;
    qDebug() << "Presentation saved to" << path;
    return true;
}

bool PresentationDocument::load()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Presentation path is empty";
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
        qWarning() << "Invalid .prex presentation format";
        return false;
    }

    fromJson(doc.object());
    m_isDirty = false;
    qDebug() << "Presentation loaded from" << m_filePath;
    return true;
}

} // namespace blastmaster::presentation
