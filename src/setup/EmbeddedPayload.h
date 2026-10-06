#pragma once

#include <QString>

class EmbeddedPayload
{
public:
    explicit EmbeddedPayload(QString executablePath);

    bool extractTo(
        const QString& directory,
        QString& error) const;

private:
    QString executablePath_;
};
