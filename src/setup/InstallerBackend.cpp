#include "InstallerBackend.h"

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTextStream>
#include <utility>

namespace
{
QString editionName(blastmaster::Edition edition)
{
    switch (edition)
    {
    case blastmaster::Edition::Standard:
        return QStringLiteral("Standard");
    case blastmaster::Edition::Professional:
        return QStringLiteral("Professional");
    case blastmaster::Edition::Invalid:
    default:
        return QStringLiteral("Invalid");
    }
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
        result.message =
            QStringLiteral("No valid Blastmaster Suite edition was selected.");
        return result;
    }

    const blastmaster::Edition keyEdition =
        blastmaster::ProductKeyValidator::detect_edition(
            productKey.toStdString());

    if (keyEdition != edition)
    {
        result.message =
            QStringLiteral(
                "The product key does not match the selected edition.");
        return result;
    }

    const QString cleanDestination = QDir::cleanPath(destination);
    if (cleanDestination.isEmpty())
    {
        result.message =
            QStringLiteral("No installation destination was specified.");
        return result;
    }

    const QString commonPayload =
        QDir(payloadRoot_).filePath(QStringLiteral("common"));
    const QString standardPayload =
        QDir(payloadRoot_).filePath(QStringLiteral("standard"));
    const QString professionalPayload =
        QDir(payloadRoot_).filePath(QStringLiteral("professional"));

    if (!QDir(standardPayload).exists())
    {
        result.message =
            QStringLiteral(
                "The installer payload is incomplete. The Standard "
                "payload is missing:\n%1")
            .arg(standardPayload);
        return result;
    }

    if (!QDir().mkpath(cleanDestination))
    {
        result.message =
            QStringLiteral(
                "Could not create the installation directory:\n%1")
            .arg(cleanDestination);
        return result;
    }

    QString error;

    if (QDir(commonPayload).exists() &&
        !copyTree(commonPayload, cleanDestination, error))
    {
        result.message = error;
        return result;
    }

    if (!copyTree(standardPayload, cleanDestination, error))
    {
        result.message = error;
        return result;
    }

    if (edition == blastmaster::Edition::Professional)
    {
        if (!QDir(professionalPayload).exists())
        {
            result.message =
                QStringLiteral(
                    "The Professional edition payload is missing:\n%1")
                .arg(professionalPayload);
            return result;
        }

        if (!copyTree(professionalPayload, cleanDestination, error))
        {
            result.message = error;
            return result;
        }
    }

    if (!writeActivation(
            cleanDestination,
            edition,
            productKey,
            error))
    {
        result.message = error;
        return result;
    }

    result.success = true;
    result.installedLocation = cleanDestination;
    result.message =
        QStringLiteral(
            "Blastmaster Suite %1 edition was installed successfully.")
        .arg(editionName(edition));

    return result;
}

bool InstallerBackend::copyTree(
    const QString& source,
    const QString& destination,
    QString& error) const
{
    QDir sourceDir(source);

    if (!sourceDir.exists())
    {
        error =
            QStringLiteral(
                "Installer payload folder does not exist:\n%1")
            .arg(source);
        return false;
    }

    QDirIterator iterator(
        source,
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        const QString sourcePath = iterator.next();
        const QFileInfo sourceInfo(sourcePath);
        const QString relativePath =
            sourceDir.relativeFilePath(sourcePath);
        const QString destinationPath =
            QDir(destination).filePath(relativePath);

        if (sourceInfo.isDir())
        {
            if (!QDir().mkpath(destinationPath))
            {
                error =
                    QStringLiteral(
                        "Could not create installation folder:\n%1")
                    .arg(destinationPath);
                return false;
            }
            continue;
        }

        const QFileInfo destinationInfo(destinationPath);

        if (!QDir().mkpath(destinationInfo.absolutePath()))
        {
            error =
                QStringLiteral(
                    "Could not create installation folder:\n%1")
                .arg(destinationInfo.absolutePath());
            return false;
        }

        if (QFile::exists(destinationPath) &&
            !QFile::remove(destinationPath))
        {
            error =
                QStringLiteral(
                    "Could not replace existing file:\n%1")
                .arg(destinationPath);
            return false;
        }

        if (!QFile::copy(sourcePath, destinationPath))
        {
            error =
                QStringLiteral(
                    "Could not copy file:\n%1\n\nto:\n%2")
                .arg(sourcePath, destinationPath);
            return false;
        }
    }

    return true;
}

bool InstallerBackend::writeActivation(
    const QString& destination,
    blastmaster::Edition edition,
    const QString& productKey,
    QString& error) const
{
    const QByteArray keyHash =
        QCryptographicHash::hash(
            productKey.toUtf8(),
            QCryptographicHash::Sha256)
            .toHex();

    const QString activationDirectory =
        QDir(destination).filePath(QStringLiteral("config"));

    if (!QDir().mkpath(activationDirectory))
    {
        error =
            QStringLiteral(
                "Could not create the configuration folder:\n%1")
            .arg(activationDirectory);
        return false;
    }

    const QString activationPath =
        QDir(activationDirectory).filePath(
            QStringLiteral("activation.ini"));

    QSaveFile file(activationPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        error =
            QStringLiteral(
                "Could not write activation information:\n%1")
            .arg(activationPath);
        return false;
    }

    QTextStream stream(&file);
    stream << "[Blastmaster Suite]\n";
    stream << "Edition=" << editionName(edition) << "\n";
    stream << "ProductKeySHA256="
           << QString::fromLatin1(keyHash)
           << "\n";

    if (!file.commit())
    {
        error =
            QStringLiteral(
                "Could not finish writing activation information:\n%1")
            .arg(activationPath);
        return false;
    }

    return true;
}
