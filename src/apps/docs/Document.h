#pragma once

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <memory>

namespace blastmaster::docs {

/**
 * @class Document
 * @brief Represents a Blastmaster Docs document with .dccx format
 */
class Document {
public:
    Document();
    explicit Document(const QString& filePath);

    // Document properties
    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; m_isDirty = true; }

    QString content() const { return m_content; }
    void setContent(const QString& content) { m_content = content; m_isDirty = true; }

    QString filePath() const { return m_filePath; }
    void setFilePath(const QString& path) { m_filePath = path; }

    bool isDirty() const { return m_isDirty; }
    void setClean() { m_isDirty = false; }

    // File operations
    bool save();
    bool saveAs(const QString& filePath);
    bool load();

    // Static factory
    static QString fileExtension() { return ".dccx"; }
    static QString fileFilter() { return "Blastmaster Documents (*.dccx);;All Files (*.*)"; }
    static QString defaultSuffix() { return "dccx"; }

private:
    QString m_title;
    QString m_content;
    QString m_filePath;
    bool m_isDirty;

    // Serialization helpers
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

} // namespace blastmaster::docs
