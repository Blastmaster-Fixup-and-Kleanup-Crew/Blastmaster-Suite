#pragma once

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <memory>

namespace blastmaster::presentation {

/**
 * @class PresentationDocument
 * @brief Represents a presentation document with .prex JSON backing storage.
 */
class PresentationDocument {
public:
    PresentationDocument();
    explicit PresentationDocument(const QString& filePath);

    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; m_isDirty = true; }

    QString content() const { return m_content; }
    void setContent(const QString& content) { m_content = content; m_isDirty = true; }

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
    QString m_content;
    QString m_filePath;
    bool m_isDirty;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

} // namespace blastmaster::presentation
