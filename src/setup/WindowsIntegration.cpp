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
bool writeRegistryString(HKEY key, const wchar_t* name, const QString& value)
{
    const std::wstring wide = value.toStdWString();
    return RegSetValueExW(
        key, name, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(wide.c_str()),
        static_cast<DWORD>((wide.size() + 1) * sizeof(wchar_t))) == ERROR_SUCCESS;
}

bool readRegistryString(HKEY root, const QString& path, const wchar_t* name, QString& value)
{
    HKEY key = nullptr;
    if (RegOpenKeyExW(
            root, reinterpret_cast<LPCWSTR>(path.utf16()), 0, KEY_READ, &key) != ERROR_SUCCESS)
        return false;

    wchar_t buffer[4096];
    DWORD size = sizeof(buffer);
    DWORD type = 0;
    const LONG result = RegQueryValueExW(
        key, name, nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &size);
    RegCloseKey(key);

    if (result != ERROR_SUCCESS || type != REG_SZ)
        return false;

    value = QString::fromWCharArray(buffer);
    return true;
}

bool deleteRegistryTree(HKEY root, const QString& path, QString& error)
{
    const LONG result = RegDeleteTreeW(
        root, reinterpret_cast<LPCWSTR>(path.utf16()));

    if (result == ERROR_SUCCESS ||
        result == ERROR_FILE_NOT_FOUND ||
        result == ERROR_PATH_NOT_FOUND)
        return true;

    error = QStringLiteral("Could not remove Windows registry data for %1.").arg(path);
    return false;
}

bool writeRegistry(
    const QString& destination,
    blastmaster::Edition edition,
    QString& error)
{
    const QString uninstallKey =
        QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Blastmaster Suite");

    HKEY key = nullptr;
    const LONG result = RegCreateKeyExW(
        HKEY_CURRENT_USER, reinterpret_cast<LPCWSTR>(uninstallKey.utf16()),
        0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &key, nullptr);

    if (result != ERROR_SUCCESS)
    {
        error = QStringLiteral("Could not register Blastmaster Suite for Windows uninstall.");
        return false;
    }

    const QString uninstallPath = QDir(destination).filePath(QStringLiteral("Uninstall.exe"));
    const QString quotedUninstall =
        QStringLiteral("\"%1\" \"%2\"")
            .arg(QDir::toNativeSeparators(uninstallPath),
                 QDir::toNativeSeparators(destination));

    const bool ok =
        writeRegistryString(key, L"DisplayName",
            QStringLiteral("Blastmaster Suite %1").arg(editionName(edition))) &&
        writeRegistryString(key, L"DisplayVersion", QStringLiteral("1.0.0.67")) &&
        writeRegistryString(key, L"Publisher",
            QStringLiteral("Blastmaster Fixup and Kleanup Crew")) &&
        writeRegistryString(key, L"InstallLocation", destination) &&
        writeRegistryString(key, L"UninstallString", quotedUninstall);

    RegCloseKey(key);

    if (!ok)
    {
        error = QStringLiteral("Could not write the Windows uninstall registration.");
        return false;
    }
    return true;
}

bool writeFileAssociation(
    const QString& extension,
    const QString& applicationName,
    const QString& executable,
    const QString& description,
    const QString& destination,
    QString& error)
{
    const QString classesRoot = QStringLiteral("Software\\Classes\\");
    const QString extensionKey = classesRoot + extension;
    const QString progId = QStringLiteral("Blastmaster.%1").arg(applicationName);
    const QString progIdKey = classesRoot + progId;

    HKEY key = nullptr;
    LONG result = RegCreateKeyExW(
        HKEY_CURRENT_USER, reinterpret_cast<LPCWSTR>(extensionKey.utf16()),
        0, nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE,
        nullptr, &key, nullptr);

    if (result != ERROR_SUCCESS)
    {
        error = QStringLiteral("Could not register the %1 file type.").arg(extension);
        return false;
    }

    wchar_t previous[1024];
    DWORD previousSize = sizeof(previous);
    DWORD type = 0;
    const LONG previousResult = RegQueryValueExW(
        key, nullptr, nullptr, &type,
        reinterpret_cast<LPBYTE>(previous), &previousSize);

    const bool alreadyOurs =
        previousResult == ERROR_SUCCESS &&
        type == REG_SZ &&
        QString::fromWCharArray(previous) == progId;

    if (!alreadyOurs)
    {
        if (previousResult == ERROR_SUCCESS && type == REG_SZ)
        {
            if (!writeRegistryString(
                    key, L"BlastmasterPreviousDefault",
                    QString::fromWCharArray(previous)))
            {
                RegCloseKey(key);
                error = QStringLiteral("Could not preserve the previous %1 association.").arg(extension);
                return false;
            }
        }
        else
        {
            RegDeleteValueW(key, L"BlastmasterPreviousDefault");
        }
    }

    const bool extensionOk = writeRegistryString(key, L"", progId);
    RegCloseKey(key);

    if (!extensionOk)
    {
        error = QStringLiteral("Could not register the %1 file type.").arg(extension);
        return false;
    }

    result = RegCreateKeyExW(
        HKEY_CURRENT_USER, reinterpret_cast<LPCWSTR>(progIdKey.utf16()),
        0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &key, nullptr);

    if (result != ERROR_SUCCESS)
    {
        error = QStringLiteral("Could not register the %1 application type.").arg(extension);
        return false;
    }

    const QString command =
        QStringLiteral("\"%1\" \"%2\"")
            .arg(QDir::toNativeSeparators(QDir(destination).filePath(executable)),
                 QStringLiteral("%1"));

    const bool progIdOk =
        writeRegistryString(key, L"", description) &&
        writeRegistryString(key, L"FriendlyTypeName", description);

    RegCloseKey(key);

    if (!progIdOk)
    {
        error = QStringLiteral("Could not register the %1 application type.").arg(extension);
        return false;
    }

    result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        reinterpret_cast<LPCWSTR>((progIdKey + QStringLiteral("\\shell\\open\\command")).utf16()),
        0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &key, nullptr);

    if (result != ERROR_SUCCESS)
    {
        error = QStringLiteral("Could not register the %1 open command.").arg(extension);
        return false;
    }

    const bool commandOk = writeRegistryString(key, L"", command);
    RegCloseKey(key);

    if (!commandOk)
    {
        error = QStringLiteral("Could not register the %1 open command.").arg(extension);
        return false;
    }
    return true;
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
    hr = link->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persist));

    bool ok = false;
    if (SUCCEEDED(hr))
    {
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
bool install(
    const QString& destination,
    blastmaster::Edition edition,
    QString& error)
{
#ifdef Q_OS_WIN
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com) && com != RPC_E_CHANGED_MODE)
    {
        error = QStringLiteral("Could not initialize Windows shell integration.");
        return false;
    }

    const QString menuFolder =
        QDir(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation))
            .filePath(QStringLiteral("Blastmaster Suite"));

    if (!QDir().mkpath(menuFolder))
    {
        if (com != RPC_E_CHANGED_MODE)
            CoUninitialize();
        error = QStringLiteral("Could not create the Blastmaster Suite Start Menu folder.");
        return false;
    }

    auto shortcut = [&](const QString& name, const QString& exe, const QString& description)
    {
        return createShortcut(
            QDir(menuFolder).filePath(name + QStringLiteral(".lnk")),
            QDir(destination).filePath(exe),
            destination,
            description);
    };

    bool ok =
        shortcut(QStringLiteral("Docs"), QStringLiteral("blastmaster_docs.exe"),
                 QStringLiteral("Blastmaster Suite Docs")) &&
        shortcut(QStringLiteral("Workbooks"), QStringLiteral("blastmaster_workbooks.exe"),
                 QStringLiteral("Blastmaster Suite Workbooks")) &&
        shortcut(QStringLiteral("Presentations"), QStringLiteral("blastmaster_presentations.exe"),
                 QStringLiteral("Blastmaster Suite Presentations"));

    if (edition == blastmaster::Edition::Professional)
    {
        ok = ok &&
            shortcut(QStringLiteral("Databases"), QStringLiteral("blastmaster_database.exe"),
                     QStringLiteral("Blastmaster Suite Databases"));
    }

    if (ok)
        ok = QFile::exists(QDir(destination).filePath(QStringLiteral("Uninstall.exe")));

    if (ok)
        ok = writeRegistry(destination, edition, error);

    if (ok)
    {
        ok =
            writeFileAssociation(QStringLiteral(".dccx"), QStringLiteral("Docs"),
                QStringLiteral("blastmaster_docs.exe"),
                QStringLiteral("Blastmaster Suite Document"), destination, error) &&
            writeFileAssociation(QStringLiteral(".wkbx"), QStringLiteral("Workbooks"),
                QStringLiteral("blastmaster_workbooks.exe"),
                QStringLiteral("Blastmaster Suite Workbook"), destination, error) &&
            writeFileAssociation(QStringLiteral(".prex"), QStringLiteral("Presentations"),
                QStringLiteral("blastmaster_presentations.exe"),
                QStringLiteral("Blastmaster Suite Presentation"), destination, error);
    }

    if (ok && edition == blastmaster::Edition::Professional)
    {
        ok = writeFileAssociation(QStringLiteral(".dbbx"), QStringLiteral("Databases"),
            QStringLiteral("blastmaster_database.exe"),
            QStringLiteral("Blastmaster Suite Database"), destination, error);
    }

    if (!ok && error.isEmpty())
        error = QStringLiteral("Could not create the Blastmaster Suite Windows integration.");

    if (com != RPC_E_CHANGED_MODE)
        CoUninitialize();

    return ok;
#else
    Q_UNUSED(destination)
    Q_UNUSED(edition)
    Q_UNUSED(error)
    return true;
#endif
}

bool uninstall(
    const QString& destination,
    QString& error)
{
#ifdef Q_OS_WIN
    const QString menuFolder =
        QDir(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation))
            .filePath(QStringLiteral("Blastmaster Suite"));

    QDir menu(menuFolder);
    if (menu.exists() && !menu.removeRecursively())
    {
        error = QStringLiteral("Could not remove the Blastmaster Suite Start Menu folder.");
        return false;
    }

    if (!deleteRegistryTree(
            HKEY_CURRENT_USER,
            QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Blastmaster Suite"),
            error))
        return false;

    const QStringList extensions = {
        QStringLiteral(".dccx"),
        QStringLiteral(".wkbx"),
        QStringLiteral(".prex"),
        QStringLiteral(".dbbx")
    };

    QStringList failures;

    for (const QString& extension : extensions)
    {
        const QString appName =
            extension == QStringLiteral(".dccx") ? QStringLiteral("Docs") :
            extension == QStringLiteral(".wkbx") ? QStringLiteral("Workbooks") :
            extension == QStringLiteral(".prex") ? QStringLiteral("Presentations") :
                                                   QStringLiteral("Databases");

        const QString extensionKey =
            QStringLiteral("Software\\Classes\\%1").arg(extension);
        const QString progId =
            QStringLiteral("Blastmaster.%1").arg(appName);
        QString currentDefault;

        if (readRegistryString(
                HKEY_CURRENT_USER, extensionKey, nullptr, currentDefault) &&
            currentDefault == progId)
        {
            QString previous;
            if (readRegistryString(
                    HKEY_CURRENT_USER, extensionKey,
                    L"BlastmasterPreviousDefault", previous))
            {
                HKEY key = nullptr;
                if (RegOpenKeyExW(
                        HKEY_CURRENT_USER,
                        reinterpret_cast<LPCWSTR>(extensionKey.utf16()),
                        0, KEY_SET_VALUE, &key) == ERROR_SUCCESS)
                {
                    const bool restored =
                        writeRegistryString(key, L"", previous) &&
                        RegDeleteValueW(key, L"BlastmasterPreviousDefault") == ERROR_SUCCESS;
                    RegCloseKey(key);
                    if (!restored)
                        failures.append(extension);
                }
            }
            else if (!deleteRegistryTree(HKEY_CURRENT_USER, extensionKey, error))
            {
                failures.append(extension);
            }
        }

        const QString progIdKey =
            QStringLiteral("Software\\Classes\\%1").arg(progId);

        QString progError;
        if (!deleteRegistryTree(HKEY_CURRENT_USER, progIdKey, progError))
            failures.append(progId);
    }

    if (!failures.isEmpty())
    {
        error = QStringLiteral("Some Windows file associations could not be fully removed: %1")
            .arg(failures.join(QStringLiteral(", ")));
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
