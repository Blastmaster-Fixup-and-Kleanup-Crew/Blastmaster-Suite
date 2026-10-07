#include "blastmaster/HelpTopicsWindow.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Blastmaster Suite Help"));
    app.setApplicationVersion(QStringLiteral("1.0.0.67"));
    app.setOrganizationName(QStringLiteral("Blastmaster"));

    QMainWindow parent;
    parent.setWindowTitle(QStringLiteral("Blastmaster Suite Help"));
    blastmaster::HelpTopicsWindow::showFor(&parent, QStringLiteral("suite"));

    return app.exec();
}
