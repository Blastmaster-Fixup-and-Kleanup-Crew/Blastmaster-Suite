#pragma once

#include "blastmaster/ProductKeyValidator.h"

#include <QString>
#include <QStringList>

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
        QStringList& createdFiles,
        QString& error) const;

    bool restoreUnmanagedFiles(
        const QString& backupRoot,
        const QString& destination,
        const QStringList& payloadRoots,
        QString& error) const;

    bool writeActivation(
        const QString& destination,
        blastmaster::Edition edition,
        const QString& productKey,
        QStringList& createdFiles,
        QString& error) const;

    bool installWindowsIntegration(
        const QString& destination,
        blastmaster::Edition edition,
        QString& error) const;

    bool rollback(
        const QString& destination,
        const QStringList& createdFiles,
        bool destinationWasCreated,
        QString& error) const;

    QString payloadRoot_;
};
