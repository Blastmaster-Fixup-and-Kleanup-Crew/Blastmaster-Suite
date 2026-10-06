#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include "blastmaster/ProductKeyValidator.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Workbooks");
    app.setApplicationDisplayName("Blastmaster Workbooks");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Workbooks");

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);

    auto* title = new QLabel("Blastmaster Workbooks", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: 600; margin-top: 24px;");
    layout->addWidget(title);

    auto* body = new QLabel("Data analysis and calculation workspace ready.", central);
    body->setAlignment(Qt::AlignCenter);
    body->setStyleSheet("font-size: 16px; margin: 12px;");
    layout->addWidget(body);

    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Standard);
    const std::string sample_key = "BMSSTD-SPREADSHEET";
    const bool valid = validator.validate(sample_key);
    auto* status = new QLabel(QString("Edition: %1 | Sample key valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No"), central);
    status->setAlignment(Qt::AlignCenter);
    status->setStyleSheet("font-size: 12px; color: #4d4d4d;");
    layout->addWidget(status);

    window.setCentralWidget(central);
    window.statusBar()->showMessage("Spreadsheet module initialized");
    window.show();

    return app.exec();
}
