#include "SetupWizard.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    application.setApplicationName(
        QStringLiteral("Blastmaster Suite Setup"));

    application.setApplicationVersion(
        QStringLiteral("0.1.0"));

    application.setOrganizationName(
        QStringLiteral("Blastmaster"));

    SetupWizard wizard;
    wizard.show();

    return application.exec();
}
