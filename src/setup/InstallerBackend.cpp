#include "InstallerBackend.h"
#include "WindowsIntegration.h"

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QSettings>
#include <QTextStream>
#include <utility>

namespace
{
QString editionName(blastmaster::Edition edition)
{
    switch (edition)
    {
    case blastmaster::Edition::Standard: return QStringLiteral("Standard");
    case blastmaster::Edition::Professional: return QStringLiteral("Professional");
    default: return QStringLiteral("Invalid");
    }
}

bool isEmptyDirectory(const QString& path)
{
    QDir dir(path);
    return dir.exists() && dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty();
}
}

InstallerBackend::InstallerBackend(QString payloadRoot)
    : payloadRoot_(QDir::cleanPath(std::move(payloadRoot)))
{
}

InstallerBackend::Result InstallerBackend::install(
    blastmaster::Edition edition,
    const QString& destination,
    const QString& productKey) const
{
    Result result;

    if (edition == blastmaster::Edition::Invalid)
    {
        result.message = QStringLiteral("No valid Blastmaster Suite edition was selected.");
        return result;
    }

    if (blastmaster::ProductKeyValidator::detect_edition(productKey.toStdString()) != edition)
    {
        result.message = QStringLiteral("The product key does not match the selected edition.");
        return result;
    }

    const QString cleanDestination = QDir::cleanPath(destination);
    if (cleanDestination.isEmpty())
    {
        result.message = QStringLiteral("No installation destination was specified.");
        return result;
    }

    const QString commonPayload = QDir(payloadRoot_).filePath(QStringLiteral("common"));
    const QString standardPayload = QDir(payloadRoot_).filePath(QStringLiteral("standard"));
    const QString professionalPayload = QDir(payloadRoot_).filePath(QStringLiteral("professional"));

    if (!QDir(standardPayload).exists())
    {
        result.message = QStringLiteral("The installer payload is incomplete. The Standard payload is missing:\n%1").arg(standardPayload);
        return result;
    }

    if (edition == blastmaster::Edition::Professional && !QDir(professionalPayload).exists())
    {
        result.message = QStringLiteral("The Professional edition payload is missing:\n%1").arg(professionalPayload);
        return result;
    }

    const QString activationPath =
        QDir(cleanDestination).filePath(QStringLiteral("config/activation.ini"));

    if (QFile::exists(activationPath))
    {
        QSettings activation(activationPath, QSettings::IniFormat);
        const QString installedEdition =
            activation.value(QStringLiteral("Blastmaster Suite/Edition")).toString();

        if (!installedEdition.isEmpty() &&
            installedEdition.compare(editionName(edition), Qt::CaseInsensitive) != 0)
        {
            result.message =
                QStringLiteral(
                    "A different Blastmaster Suite edition is already installed in this folder (%1). "
                    "Choose another destination or uninstall the existing edition first.")
                .arg(installedEdition);
            return result;
        }
    }

    const bool destinationExisted = QDir(cleanDestination).exists();
    const bool destinationWasEmpty = destinationExisted && isEmptyDirectory(cleanDestination);
    const QString backupDestination = cleanDestination + QStringLiteral(".blastmaster-backup");

    if (QDir(backupDestination).exists())
        QDir(backupDestination).removeRecursively();

    bool backedUpExisting = false;
    if (destinationExisted && !destinationWasEmpty)
    {
        if (!QDir().rename(cleanDestination, backupDestination))
        {
            result.message =
                QStringLiteral(
                    "Could not prepare the existing installation for a safe update. "
                    "Close Blastmaster Suite and try again.");
            return result;
        }
        backedUpExisting = true;
    }

    if (!QDir().mkpath(cleanDestination))
    {
        if (backedUpExisting)
            QDir().rename(backupDestination, cleanDestination);
        result.message = QStringLiteral("Could not create the installation directory:\n%1").arg(cleanDestination);
        return result;
    }

    QStringList createdFiles;
    QString error;

    auto fail = [&](const QString& message)
    {
        QString rollbackError;
        rollback(cleanDestination, createdFiles, !destinationExisted || destinationWasEmpty, rollbackError);

        if (backedUpExisting)
        {
            QDir(cleanDestination).removeRecursively();
            QDir().rename(backupDestination, cleanDestination);
        }

        result.message = message;
        if (!rollbackError.isEmpty())
            result.message += QStringLiteral("\n\nCleanup warning: ") + rollbackError;
        return result;
    };

    if (QDir(commonPayload).exists() &&
        !copyTree(commonPayload, cleanDestination, createdFiles, error))
        return fail(error);

    if (!copyTree(standardPayload, cleanDestination, createdFiles, error))
        return fail(error);

    if (edition == blastmaster::Edition::Professional &&
        !copyTree(professionalPayload, cleanDestination, createdFiles, error))
        return fail(error);

    if (!writeActivation(cleanDestination, edition, productKey, createdFiles, error))
        return fail(error);

    if (!installWindowsIntegration(cleanDestination, edition, error))
        return fail(error);

    if (backedUpExisting)
        QDir(backupDestination).removeRecursively();

    result.success = true;
    result.installedLocation = cleanDestination;
    result.message =
        QStringLiteral("Blastmaster Suite %1 edition was installed successfully.")
        .arg(editionName(edition));
    return result;
}

bool InstallerBackend::copyTree(
    const QString& source,
    const QString& destination,
    QStringList& createdFiles,
    QString& error) const
{
    QDir sourceDir(source);
    if (!sourceDir.exists())
    {
        error = QStringLiteral("Installer payload folder does not exist:\n%1").arg(source);
        return false;
    }

    QDirIterator iterator(
        source, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        const QString sourcePath = iterator.next();
        const QFileInfo sourceInfo(sourcePath);
        const QString relativePath = sourceDir.relativeFilePath(sourcePath);
        const QString destinationPath = QDir(destination).filePath(relativePath);

        if (sourceInfo.isDir())
        {
            if (!QDir().mkpath(destinationPath))
            {
                error = QStringLiteral("Could not create installation folder:\n%1").arg(destinationPath);
                return false;
            }
            continue;
        }

        const QFileInfo destinationInfo(destinationPath);
        if (!QDir().mkpath(destinationInfo.absolutePath()))
        {
            error = QStringLiteral("Could not create installation folder:\n%1").arg(destinationInfo.absolutePath());
            return false;
        }

        if (QFile::exists(destinationPath) && !QFile::remove(destinationPath))
        {
            error = QStringLiteral("Could not replace existing file:\n%1").arg(destinationPath);
            return false;
        }

        if (!QFile::copy(sourcePath, destinationPath))
        {
            error = QStringLiteral("Could not copy file:\n%1\n\nto:\n%2")
                .arg(sourcePath, destinationPath);
            return false;
        }

        createdFiles.append(destinationPath);
    }

    return true;
}

bool InstallerBackend::writeActivation(
    const QString& destination,
    blastmaster::Edition edition,
    const QString& productKey,
    QStringList& createdFiles,
    QString& error) const
{
    const QString activationDirectory =
        QDir(destination).filePath(QStringLiteral("config"));
    if (!QDir().mkpath(activationDirectory))
    {
        error = QStringLiteral("Could not create the configuration folder:\n%1").arg(activationDirectory);
        return false;
    }

    const QString activationPath =
        QDir(activationDirectory).filePath(QStringLiteral("activation.ini"));

    QSaveFile file(activationPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        error = QStringLiteral("Could not write activation information:\n%1").arg(activationPath);
        return false;
    }

    const QByteArray keyHash =
        QCryptographicHash::hash(productKey.toUtf8(), QCryptographicHash::Sha256).toHex();

    QTextStream stream(&file);
    stream << "[Blastmaster Suite]\n";
    stream << "Edition=" << editionName(edition) << "\n";
    stream << "ProductKeySHA256=" << QString::fromLatin1(keyHash) << "\n";

    if (!file.commit())
    {
        error = QStringLiteral("Could not finish writing activation information:\n%1").arg(activationPath);
        return false;
    }

    createdFiles.append(activationPath);
    return true;
}

bool InstallerBackend::installWindowsIntegration(
    const QString& destination,
    blastmaster::Edition edition,
    QString& error) const
{
    return WindowsIntegration::install(destination, edition, error);
}

bool InstallerBackend::rollback(
    const QString& destination,
    const QStringList& createdFiles,
    bool destinationWasCreated,
    QString& error) const
{
    QStringList failures;

    for (auto it = createdFiles.crbegin(); it != createdFiles.crend(); ++it)
    {
        if (QFile::exists(*it) && !QFile::remove(*it))
            failures.append(*it);
    }

    if (destinationWasCreated && QDir(destination).exists())
    {
        QDir dir(destination);
        if (!dir.removeRecursively())
            failures.append(destination);
    }

    if (!failures.isEmpty())
    {
        error = QStringLiteral("Some temporary installation files could not be removed:\n%1")
            .arg(failures.join(QStringLiteral("\n")));
        return false;
    }

    return true;
}
