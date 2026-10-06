#include "SetupWizard.h"
#include "EmbeddedPayload.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QTemporaryDir>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    application.setApplicationName(
        QStringLiteral("Blastmaster Suite Setup"));

    application.setApplicationVersion(
        QStringLiteral("0.1.0"));

    application.setOrganizationName(
        QStringLiteral("Blastmaster"));

#ifdef Q_OS_WIN
    QTemporaryDir payloadDirectory;

    if (!payloadDirectory.isValid())
    {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("Blastmaster Suite Setup"),
            QStringLiteral(
                "Setup could not create its temporary workspace."));
        return 1;
    }

    EmbeddedPayload payload(
        QCoreApplication::applicationFilePath());

    QString payloadError;

    if (!payload.extractTo(
            payloadDirectory.path(),
            payloadError))
    {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("Blastmaster Suite Setup"),
            payloadError);
        return 1;
    }

    qputenv(
        "BLASTMASTER_SETUP_PAYLOAD",
        QDir::toNativeSeparators(
            payloadDirectory.path()).toUtf8());
#endif

    SetupWizard wizard;
    wizard.show();

    return application.exec();
}
