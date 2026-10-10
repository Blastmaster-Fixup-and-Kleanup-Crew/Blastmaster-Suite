#include "SetupWizard.h"
#include "EmbeddedPayload.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QLocale>
#include <QTranslator>
#include <QTemporaryDir>

#ifdef Q_OS_WIN
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <shellapi.h>
#  include <sddl.h>
#  include <vector>
#  include <string>

namespace
{
bool isProcessElevated()
{
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return false;

    TOKEN_ELEVATION elevation{};
    DWORD size = 0;
    const BOOL ok = GetTokenInformation(token, TokenElevation, &elevation,
                                        sizeof(elevation), &size);
    CloseHandle(token);
    return ok && elevation.TokenIsElevated != 0;
}

std::wstring quoteWindowsArgument(const std::wstring& arg)
{
    if (arg.find_first_of(L" \t\n\v\"") == std::wstring::npos)
        return arg;

    std::wstring quoted = L"\"";
    unsigned int backslashes = 0;
    for (wchar_t ch : arg) {
        if (ch == L'\\') {
            ++backslashes;
        } else if (ch == L'\"') {
            quoted.append(backslashes * 2 + 1, L'\\');
            quoted.push_back(L'\"');
            backslashes = 0;
        } else {
            quoted.append(backslashes, L'\\');
            backslashes = 0;
            quoted.push_back(ch);
        }
    }
    quoted.append(backslashes * 2, L'\\');
    quoted.push_back(L'\"');
    return quoted;
}

bool relaunchElevated(int argc, char* argv[])
{
    if (isProcessElevated())
        return true;

    wchar_t executable[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, executable, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return false;

    std::wstring parameters;
    for (int i = 1; i < argc; ++i) {
        if (!parameters.empty())
            parameters.push_back(L' ');
        const QString arg = QString::fromLocal8Bit(argv[i]);
        parameters += quoteWindowsArgument(arg.toStdWString());
    }

    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = L"runas";
    info.lpFile = executable;
    info.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
    info.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&info))
        return false; // Includes a user cancelling the UAC prompt.

    if (info.hProcess)
        CloseHandle(info.hProcess);
    return true;
}
}
#endif

int main(int argc, char* argv[])
{
#ifdef Q_OS_WIN
    // Keep the manifest as the primary elevation mechanism. This is a fallback
    // for launch/build scenarios where Windows did not honor the embedded manifest.
    if (!isProcessElevated()) {
        if (relaunchElevated(argc, argv))
            return 0;

        const DWORD error = GetLastError();
        if (error == ERROR_CANCELLED)
            return 1;

        MessageBoxW(nullptr,
                    L"Blastmaster Suite Setup requires administrator permission. "
                    L"Please approve the Windows elevation prompt and try again.",
                    L"Blastmaster Suite Setup",
                    MB_OK | MB_ICONERROR);
        return 1;
    }
#endif

    QApplication application(argc, argv);

    QTranslator setupTranslator;
    if (setupTranslator.load(QLocale::system(), QStringLiteral("blastmaster_setup"), QStringLiteral("_"), QStringLiteral(":/i18n")))
        application.installTranslator(&setupTranslator);

    application.setApplicationName(QStringLiteral("Blastmaster Suite Setup"));
    application.setApplicationVersion(QStringLiteral("1.0.0.67"));
    application.setOrganizationName(QStringLiteral("Blastmaster"));

#ifdef Q_OS_WIN
    QTemporaryDir payloadDirectory;
    if (!payloadDirectory.isValid())
    {
        QMessageBox::critical(nullptr, QStringLiteral("Blastmaster Suite Setup"),
                              QStringLiteral("Setup could not create its temporary workspace."));
        return 1;
    }

    EmbeddedPayload payload(QCoreApplication::applicationFilePath());
    QString payloadError;
    if (!payload.extractTo(payloadDirectory.path(), payloadError))
    {
        QMessageBox::critical(nullptr, QStringLiteral("Blastmaster Suite Setup"), payloadError);
        return 1;
    }

    qputenv("BLASTMASTER_SETUP_PAYLOAD",
            QDir::toNativeSeparators(payloadDirectory.path()).toUtf8());
    qputenv("BLASTMASTER_SETUP_BRANDING",
            QDir(payloadDirectory.path()).filePath(QStringLiteral("common/blastmaster_suite_setup.svg")).toUtf8());
#endif

    SetupWizard wizard;
    wizard.show();
    return application.exec();
}
