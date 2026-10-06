#pragma once

#include "blastmaster/ProductKeyValidator.h"

#include <QString>

class InstallerBackend
{
public:
    struct Result
    {
        bool success = false;
        QString message;
        QString installedLocation;
    };

    explicit InstallerBackend(QString payloadRoot);

    Result install(
        blastmaster::Edition edition,
        const QString& destination,
        const QString& productKey) const;

private:
    bool copyTree(
        const QString& source,
        const QString& destination,
        QString& error) const;

    bool writeActivation(
        const QString& destination,
        blastmaster::Edition edition,
        const QString& productKey,
        QString& error) const;

    QString payloadRoot_;
};
