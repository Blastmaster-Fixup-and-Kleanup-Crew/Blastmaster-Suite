#include "SetupWizard.h"
#include "EmbeddedPayload.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QLocale>
#include <QTranslator>
#include <QTemporaryDir>

int main(int argc, char* argv[])
{
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
