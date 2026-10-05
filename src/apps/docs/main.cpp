#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include "blastmaster/ProductKeyValidator.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Docs");
    app.setApplicationDisplayName("Blastmaster Docs");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Docs");

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);

    auto* title = new QLabel("Blastmaster Docs", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: 600; margin-top: 24px;");
    layout->addWidget(title);

    auto* body = new QLabel("Professional document editing workspace ready.", central);
    body->setAlignment(Qt::AlignCenter);
    body->setStyleSheet("font-size: 16px; margin: 12px;");
    layout->addWidget(body);

    auto* status = new QLabel("Edition: Standard | Feature set enabled", central);
    status->setAlignment(Qt::AlignCenter);
    status->setStyleSheet("font-size: 12px; color: #4d4d4d;");
    layout->addWidget(status);

    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Standard);
    const std::string sample_key = "BMSSTD-2026-WORD";
    const bool valid = validator.validate(sample_key);
    status->setText(QString("Edition: %1 | Sample key valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No"));

    window.setCentralWidget(central);
    window.statusBar()->showMessage("Word processor initialized");
    window.show();

    return app.exec();
}
