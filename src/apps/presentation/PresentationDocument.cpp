#include "PresentationDocument.h"

#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSaveFile>
#include <QDebug>

namespace blastmaster::presentation {

PresentationDocument::PresentationDocument()
    : m_title("Untitled Presentation"), m_filePath(), m_isDirty(false), m_currentSlide(0)
{
    addSlide();
    setClean();
}

PresentationDocument::PresentationDocument(const QString& filePath)
    : m_title("Untitled Presentation"), m_filePath(filePath), m_isDirty(false), m_currentSlide(0)
{
    addSlide();
    setClean();
}

QString PresentationDocument::content() const
{
    QStringList parts;
    for (const auto& slide : m_slides)
        parts << slide.title + QStringLiteral("\n") + slide.body;
    return parts.join(QStringLiteral("\n\n"));
}

void PresentationDocument::setContent(const QString& content)
{
    if (m_slides.isEmpty())
        addSlide();
    m_slides[0].body = content;
    m_isDirty = true;
}

void PresentationDocument::setCurrentSlide(int index)\n{\n    if (index >= 0 && index < m_slides.size()) m_currentSlide = index;\n}\n\nvoid PresentationDocument::setSlide(int index, const QString& title, const QString& body)
{
    if (index < 0 || index >= m_slides.size()) return;
    m_slides[index].title = title;
    m_slides[index].body = body;
    m_isDirty = true;
}

void PresentationDocument::addSlide(const QString& title, const QString& body)
{
    m_slides.append({title, body});
    m_isDirty = true;
}

void PresentationDocument::removeSlide(int index)
{
    if (index < 0 || index >= m_slides.size() || m_slides.size() == 1) return;
    m_slides.removeAt(index);\n    if (m_currentSlide >= m_slides.size()) m_currentSlide = m_slides.size() - 1;
    m_isDirty = true;
}

void PresentationDocument::clearSlides()
{
    m_slides.clear();
    addSlide();
    m_currentSlide = 0;
    m_isDirty = true;
}

QJsonObject PresentationDocument::toJson() const
{
    QJsonObject root;
    root["format"] = "Blastmaster.Presentation";
    root["version"] = "1.0";
    root["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["title"] = m_title;

    QJsonArray slides;
    for (const auto& value : m_slides) {
        QJsonObject slide;
        slide["title"] = value.title;
        slide["body"] = value.body;
        slides.append(slide);
    }
    root["slides"] = slides;
    return root;
}

void PresentationDocument::fromJson(const QJsonObject& json)
{
    m_slides.clear();
    m_title = json.value("title").toString(m_title);

    const QJsonValue slides = json.value("slides");
    if (slides.isArray()) {
        for (const QJsonValue& value : slides.toArray()) {
            const QJsonObject slide = value.toObject();
            m_slides.append({slide.value("title").toString(), slide.value("body").toString()});
        }
    }

    // Backward compatibility with the original single-content prototype.
    if (m_slides.isEmpty()) {
        const QString oldContent = json.value("content").toString();
        m_slides.append({QStringLiteral("Title"), oldContent});
    }
    m_currentSlide = 0;
}

bool PresentationDocument::save()
{
    if (m_filePath.isEmpty()) return false;
    return saveAs(m_filePath);
}

bool PresentationDocument::saveAs(const QString& filePath)
{
    if (filePath.isEmpty()) return false;

    QString path = filePath;
    if (!path.endsWith(fileExtension(), Qt::CaseInsensitive))
        path += fileExtension();

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;

    const QByteArray data = QJsonDocument(toJson()).toJson(QJsonDocument::Indented);
    if (file.write(data) != data.size() || !file.commit()) return false;

    m_filePath = path;
    m_isDirty = false;
    qDebug() << "Presentation saved to" << path;
    return true;
}

bool PresentationDocument::load()
{
    if (m_filePath.isEmpty()) return false;

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (!doc.isObject()) {
        qWarning() << "Invalid .prex presentation format:" << error.errorString();
        return false;
    }

    fromJson(doc.object());
    m_isDirty = false;
    return true;
}

} // namespace blastmaster::presentation
