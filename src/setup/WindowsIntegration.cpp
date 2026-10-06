#include "WindowsIntegration.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>
#endif

namespace
{
QString editionName(blastmaster::Edition edition)
{
    return edition == blastmaster::Edition::Professional
        ? QStringLiteral("Professional")
        : QStringLiteral("Standard");
}

#ifdef Q_OS_WIN
bool writeRegistry(const QString& destination, blastmaster::Edition edition, QString& error)
{
    const QString keyName =
        QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Blastmaster Suite");

    HKEY key = nullptr;
    const LONG result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        reinterpret_cast<LPCWSTR>(keyName.utf16()),
        0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE,
        nullptr, &key, nullptr);

    if (result != ERROR_SUCCESS) {
        error = QStringLiteral("Could not register Blastmaster Suite for Windows uninstall.");
        return false;
    }

    auto setString = [&](const wchar_t* name, const QString& value) {
        const std::wstring wide = value.toStdWString();
        return RegSetValueExW(
            key, name, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(wide.c_str()),
            static_cast<DWORD>((wide.size() + 1) * sizeof(wchar_t))) == ERROR_SUCCESS;
    };

    const QString uninstall =
        QDir(destination).filePath(QStringLiteral("Uninstall.exe"));

    const bool ok =
        setString(L"DisplayName", QStringLiteral("Blastmaster Suite %1").arg(editionName(edition))) &&
        setString(L"DisplayVersion", QStringLiteral("1.0")) &&
        setString(L"Publisher", QStringLiteral("Blastmaster Fixup and Kleanup Crew")) &&
        setString(L"InstallLocation", destination) &&
        setString(L"UninstallString", uninstall);

    RegCloseKey(key);

    if (!ok)
        error = QStringLiteral("Could not write the Windows uninstall registration.");

    return ok;
}

bool createShortcut(
    const QString& linkPath,
    const QString& targetPath,
    const QString& workingDirectory,
    const QString& description)
{
    IShellLinkW* link = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
        IID_IShellLinkW, reinterpret_cast<void**>(&link));

    if (FAILED(hr))
        return false;

    const std::wstring target = targetPath.toStdWString();
    const std::wstring work = workingDirectory.toStdWString();
    const std::wstring desc = description.toStdWString();

    link->SetPath(target.c_str());
    link->SetWorkingDirectory(work.c_str());
    link->SetDescription(desc.c_str());

    IPersistFile* persist = nullptr;
    hr = link->QueryInterface(
        IID_IPersistFile, reinterpret_cast<void**>(&persist));

    bool ok = false;
    if (SUCCEEDED(hr)) {
        const std::wstring output = linkPath.toStdWString();
        ok = SUCCEEDED(persist->Save(output.c_str(), TRUE));
        persist->Release();
    }

    link->Release();
    return ok;
}
#endif
}

namespace WindowsIntegration
{
bool install(const QString& destination, blastmaster::Edition edition, QString& error)
{
#ifdef Q_OS_WIN
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com) && com != RPC_E_CHANGED_MODE) {
        error = QStringLiteral("Could not initialize Windows shell integration.");
        return false;
    }

    const QString menuFolder =
        QDir(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation))
            .filePath(QStringLiteral("Blastmaster Suite"));

    if (!QDir().mkpath(menuFolder)) {
        if (com != RPC_E_CHANGED_MODE) CoUninitialize();
        error = QStringLiteral("Could not create the Blastmaster Suite Start Menu folder.");
        return false;
    }

    auto shortcut = [&](const QString& name, const QString& exe, const QString& description) {
        return createShortcut(
            QDir(menuFolder).filePath(name + QStringLiteral(".lnk")),
            QDir(destination).filePath(exe),
            destination,
            description);
    };

    bool ok =
        shortcut(QStringLiteral("Docs"), QStringLiteral("blastmaster_docs.exe"), QStringLiteral("Blastmaster Suite Docs")) &&
        shortcut(QStringLiteral("Workbooks"), QStringLiteral("blastmaster_workbooks.exe"), QStringLiteral("Blastmaster Suite Workbooks")) &&
        shortcut(QStringLiteral("Presentations"), QStringLiteral("blastmaster_presentations.exe"), QStringLiteral("Blastmaster Suite Presentations"));

    if (edition == blastmaster::Edition::Professional)
        ok = ok && shortcut(QStringLiteral("Databases"), QStringLiteral("blastmaster_database.exe"), QStringLiteral("Blastmaster Suite Databases"));

    if (ok)
        ok = QFile::exists(QDir(destination).filePath(QStringLiteral("Uninstall.exe")));

    if (ok)
        ok = writeRegistry(destination, edition, error);

    if (!ok && error.isEmpty())
        error = QStringLiteral("Could not create the Blastmaster Suite Start Menu entries.");

    if (com != RPC_E_CHANGED_MODE) CoUninitialize();
    return ok;
#else
    Q_UNUSED(destination)
    Q_UNUSED(edition)
    Q_UNUSED(error)
    return true;
#endif
}

bool uninstall(const QString& destination, QString& error)
{
#ifdef Q_OS_WIN
    const QString menuFolder =
        QDir(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation))
            .filePath(QStringLiteral("Blastmaster Suite"));

    QDir menu(menuFolder);
    if (menu.exists() && !menu.removeRecursively()) {
        error = QStringLiteral("Could not remove the Blastmaster Suite Start Menu folder.");
        return false;
    }

    const QString keyName =
        QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Blastmaster Suite");

    const LONG result = RegDeleteTreeW(
        HKEY_CURRENT_USER,
        reinterpret_cast<LPCWSTR>(keyName.utf16()));

    if (result != ERROR_SUCCESS &&
        result != ERROR_FILE_NOT_FOUND &&
        result != ERROR_PATH_NOT_FOUND) {
        error = QStringLiteral("Could not remove the Windows uninstall registration.");
        return false;
    }

    Q_UNUSED(destination)
    return true;
#else
    Q_UNUSED(destination)
    Q_UNUSED(error)
    return true;
#endif
}
}
