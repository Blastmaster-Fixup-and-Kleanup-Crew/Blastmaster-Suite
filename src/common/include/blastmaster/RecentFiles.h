#pragma once

#include <QFileInfo>
#include <QSettings>
#include <QStringList>

namespace blastmaster {

class RecentFiles {
public:
    static QStringList files(const QString& applicationId) {
        QSettings settings;
        const QStringList stored = settings.value(settingsGroup(applicationId) + QStringLiteral("/files")).toStringList();
        QStringList result;
        for (const QString& path : stored)
            if (!path.isEmpty() && QFileInfo::exists(path) && !result.contains(path))
                result.append(path);
        return result;
    }

    static void add(const QString& applicationId, const QString& filePath) {
        if (filePath.isEmpty())
            return;
        QStringList result{QFileInfo(filePath).absoluteFilePath()};
        for (const QString& path : files(applicationId)) {
            if (path != result.first())
                result.append(path);
            if (result.size() >= maximumEntries())
                break;
        }
        QSettings settings;
        settings.setValue(settingsGroup(applicationId) + QStringLiteral("/files"), result);
        settings.sync();
    }

    static void clear(const QString& applicationId) {
        QSettings settings;
        settings.remove(settingsGroup(applicationId));
        settings.sync();
    }

    static int maximumEntries() { return 10; }

private:
    static QString settingsGroup(const QString& applicationId) {
        return QStringLiteral("RecentFiles/") + applicationId;
    }
};

} // namespace blastmaster
