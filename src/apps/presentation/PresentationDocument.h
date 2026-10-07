#pragma once

#include <QJsonObject>
#include <QString>
#include <QStringList>

namespace blastmaster::presentation {

class PresentationDocument {
public:
    struct Slide {
        QString title;
        QString body;
    };

    PresentationDocument();
    explicit PresentationDocument(const QString& filePath);

    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; m_isDirty = true; }

    QString content() const;
    void setContent(const QString& content);

    int slideCount() const { return m_slides.size(); }
    const Slide& slide(int index) const { return m_slides.at(index); }
    void setSlide(int index, const QString& title, const QString& body);
    void addSlide(const QString& title = QStringLiteral("Title"), const QString& body = QStringLiteral("Click to add text"));
    void removeSlide(int index);
    void clearSlides();

    QString filePath() const { return m_filePath; }
    void setFilePath(const QString& path) { m_filePath = path; }

    bool isDirty() const { return m_isDirty; }
    void setClean() { m_isDirty = false; }

    bool save();
    bool saveAs(const QString& filePath);
    bool load();

    static QString fileExtension() { return ".prex"; }
    static QString fileFilter() { return "Blastmaster Presentations (*.prex);;All Files (*.*)"; }
    static QString defaultSuffix() { return "prex"; }

private:
    QString m_title;
    QList<Slide> m_slides;
    QString m_filePath;
    bool m_isDirty;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

} // namespace blastmaster::presentation
