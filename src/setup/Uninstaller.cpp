#include "WindowsIntegration.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QString destination =
        argc > 1
            ? QString::fromLocal8Bit(argv[1])
            : QCoreApplication::applicationDirPath();

    destination = QDir::cleanPath(destination);

    if (QMessageBox::question(
            nullptr,
            QStringLiteral("Blastmaster Suite"),
            QStringLiteral(
                "Are you sure you want to uninstall Blastmaster Suite?\n\n"
                "This will remove the installed applications and Start Menu shortcuts."),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No) != QMessageBox::Yes)
        return 0;

    QString error;
    if (!WindowsIntegration::uninstall(destination, error)) {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("Uninstall Failed"),
            error);
        return 1;
    }

    const QString command =
        QStringLiteral("ping 127.0.0.1 -n 2 > nul & rmdir /s /q "%1"")
            .arg(QDir::toNativeSeparators(destination));

    QProcess::startDetached(
        QStringLiteral("cmd.exe"),
        {QStringLiteral("/C"), command});

    return 0;
}
