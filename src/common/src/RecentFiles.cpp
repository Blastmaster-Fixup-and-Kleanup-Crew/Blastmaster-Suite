#include "blastmaster/RecentFiles.h"
#include <QFileInfo>
#include <QSettings>
namespace blastmaster {
QString RecentFiles::settingsGroup(const QString& applicationId) { return QStringLiteral("RecentFiles/") + applicationId; }
int RecentFiles::maximumEntries() { return 10; }
QStringList RecentFiles::files(const QString& applicationId) {
    QSettings settings;
    const QStringList stored = settings.value(settingsGroup(applicationId) + QStringLiteral("/files")).toStringList();
    QStringList result;
    for (const QString& path : stored)
        if (!path.isEmpty() && QFileInfo::exists(path) && !result.contains(path)) result.append(path);
    return result;
}
void RecentFiles::add(const QString& applicationId, const QString& filePath) {
    if (filePath.isEmpty()) return;
    QStringList result{QFileInfo(filePath).absoluteFilePath()};
    for (const QString& path : files(applicationId)) {
        if (path != result.first()) result.append(path);
        if (result.size() >= maximumEntries()) break;
    }
    QSettings settings;
    settings.setValue(settingsGroup(applicationId) + QStringLiteral("/files"), result);
    settings.sync();
}
void RecentFiles::clear(const QString& applicationId) {
    QSettings settings;
    settings.remove(settingsGroup(applicationId));
    settings.sync();
}
} // namespace blastmaster
