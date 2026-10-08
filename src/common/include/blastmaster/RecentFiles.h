#pragma once
#include <QStringList>
namespace blastmaster {
class RecentFiles {
public:
    static QStringList files(const QString& applicationId);
    static void add(const QString& applicationId, const QString& filePath);
    static void clear(const QString& applicationId);
    static int maximumEntries();
private:
    static QString settingsGroup(const QString& applicationId);
};
} // namespace blastmaster
