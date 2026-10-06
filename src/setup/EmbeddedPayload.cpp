#include "EmbeddedPayload.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTextStream>

#include <cstring>

namespace
{
constexpr char MAGIC[] = "BMSPAY01";
constexpr qint64 HEADER_SIZE = 16;

bool readExact(
    QFile& file,
    qint64 offset,
    char* buffer,
    qint64 size)
{
    if (!file.seek(offset))
        return false;

    return file.read(buffer, size) == size;
}

quint32 readU32(const char* p)
{
    return static_cast<quint32>(
        static_cast<unsigned char>(p[0])) |
        (static_cast<quint32>(
             static_cast<unsigned char>(p[1])) << 8) |
        (static_cast<quint32>(
             static_cast<unsigned char>(p[2])) << 16) |
        (static_cast<quint32>(
             static_cast<unsigned char>(p[3])) << 24);
}

quint64 readU64(const char* p)
{
    quint64 value = 0;

    for (int i = 0; i < 8; ++i)
    {
        value |=
            static_cast<quint64>(
                static_cast<unsigned char>(p[i]))
            << (i * 8);
    }

    return value;
}
}

EmbeddedPayload::EmbeddedPayload(QString executablePath)
    : executablePath_(std::move(executablePath))
{
}

bool EmbeddedPayload::extractTo(
    const QString& directory,
    QString& error) const
{
    QFile file(executablePath_);

    if (!file.open(QIODevice::ReadOnly))
    {
        error =
            QStringLiteral(
                "Could not open the Setup executable payload.");
        return false;
    }

    const qint64 fileSize = file.size();

    if (fileSize < HEADER_SIZE)
    {
        error =
            QStringLiteral(
                "This Setup executable does not contain an embedded payload.");
        return false;
    }

    // The final 16 bytes contain:
    //   8 bytes  magic
    //   8 bytes  payload start offset
    char footer[HEADER_SIZE];

    if (!readExact(
            file,
            fileSize - HEADER_SIZE,
            footer,
            HEADER_SIZE))
    {
        error =
            QStringLiteral(
                "Could not read the embedded Setup payload header.");
        return false;
    }

    if (std::memcmp(footer, MAGIC, 8) != 0)
    {
        error =
            QStringLiteral(
                "This Setup executable does not contain a valid embedded payload.");
        return false;
    }

    const quint64 payloadOffset = readU64(footer + 8);

    if (payloadOffset >= static_cast<quint64>(fileSize - HEADER_SIZE))
    {
        error =
            QStringLiteral(
                "The embedded Setup payload is corrupt.");
        return false;
    }

    if (!QDir().mkpath(directory))
    {
        error =
            QStringLiteral(
                "Could not create the temporary Setup payload directory:
%1")
            .arg(directory);
        return false;
    }

    if (!file.seek(static_cast<qint64>(payloadOffset)))
    {
        error =
            QStringLiteral(
                "Could not seek to the embedded Setup payload.");
        return false;
    }

    char archiveHeader[8];

    while (file.pos() < fileSize - HEADER_SIZE)
    {
        const qint64 remaining =
            fileSize - HEADER_SIZE - file.pos();

        if (remaining < 20)
        {
            error =
                QStringLiteral(
                    "The embedded Setup payload is corrupt.");
            return false;
        }

        char entryHeader[20];

        if (file.read(entryHeader, sizeof(entryHeader))
            != sizeof(entryHeader))
        {
            error =
                QStringLiteral(
                    "Could not read an embedded payload entry.");
            return false;
        }

        const quint32 pathLength = readU32(entryHeader);
        const quint64 dataLength = readU64(entryHeader + 4);
        const quint64 expectedSize = readU64(entryHeader + 12);

        if (pathLength == 0 ||
            pathLength > 1024 * 1024 ||
            dataLength != expectedSize)
        {
            error =
                QStringLiteral(
                    "The embedded Setup payload contains an invalid entry.");
            return false;
        }

        const QByteArray pathBytes =
            file.read(pathLength);

        if (pathBytes.size() != static_cast<int>(pathLength))
        {
            error =
                QStringLiteral(
                    "Could not read an embedded payload filename.");
            return false;
        }

        const QString relativePath =
            QString::fromUtf8(pathBytes);

        const QString outputPath =
            QDir(directory).filePath(relativePath);

        const QString canonicalRoot =
            QFileInfo(directory).canonicalFilePath();

        const QString parent =
            QFileInfo(outputPath).absolutePath();

        if (!QDir().mkpath(parent))
        {
            error =
                QStringLiteral(
                    "Could not create an embedded payload directory:
%1")
                .arg(parent);
            return false;
        }

        if (dataLength >
            static_cast<quint64>(std::numeric_limits<qint64>::max()))
        {
            error =
                QStringLiteral(
                    "An embedded payload file is too large.");
            return false;
        }

        QFile output(outputPath);

        if (!output.open(QIODevice::WriteOnly))
        {
            error =
                QStringLiteral(
                    "Could not extract embedded payload file:
%1")
                .arg(outputPath);
            return false;
        }

        quint64 remainingData = dataLength;
        QByteArray buffer(1024 * 1024, Qt::Uninitialized);

        while (remainingData > 0)
        {
            const qint64 chunk =
                static_cast<qint64>(
                    qMin<quint64>(
                        remainingData,
                        static_cast<quint64>(buffer.size())));

            const qint64 read =
                file.read(buffer.data(), chunk);

            if (read != chunk)
            {
                output.close();
                error =
                    QStringLiteral(
                        "Could not read embedded payload data.");
                return false;
            }

            if (output.write(buffer.constData(), read) != read)
            {
                output.close();
                error =
                    QStringLiteral(
                        "Could not extract embedded payload data.");
                return false;
            }

            remainingData -= static_cast<quint64>(read);
        }

        output.close();

        if (QFileInfo(outputPath).size() !=
            static_cast<qint64>(dataLength))
        {
            error =
                QStringLiteral(
                    "Extracted payload file has an unexpected size:
%1")
                .arg(outputPath);
            return false;
        }
    }

    Q_UNUSED(archiveHeader)
    Q_UNUSED(canonicalRoot)

    return true;
}
