#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include "blastmaster/ProductKeyValidator.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Database");
    app.setApplicationDisplayName("Blastmaster Database");

    QMainWindow window;
    window.resize(1100, 700);
    window.setWindowTitle("Blastmaster Database");

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);

    auto* title = new QLabel("Blastmaster Database", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: 600; margin-top: 24px;");
    layout->addWidget(title);

    auto* body = new QLabel("Professional relational database tools ready.", central);
    body->setAlignment(Qt::AlignCenter);
    body->setStyleSheet("font-size: 16px; margin: 12px;");
    layout->addWidget(body);

    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Professional);
    const std::string sample_key = "BMSPRO-DATABASE";
    const bool valid = validator.validate(sample_key);
    auto* status = new QLabel(QString("Edition: %1 | Sample key valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No"), central);
    status->setAlignment(Qt::AlignCenter);
    status->setStyleSheet("font-size: 12px; color: #4d4d4d;");
    layout->addWidget(status);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return 1;
    }

    window.setCentralWidget(central);
    window.statusBar()->showMessage("Database module initialized with SQLite");
    window.show();

    return app.exec();
}
