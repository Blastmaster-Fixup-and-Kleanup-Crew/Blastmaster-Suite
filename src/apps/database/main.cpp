#include <QApplication>
#include <QHeaderView>
#include <QMainWindow>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include "Access95MenuBar.h"
#include "Database.h"
#include "blastmaster/ProductKeyValidator.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Databases");
    app.setApplicationDisplayName("Blastmaster Databases");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Databases - Database1");

    auto* database = new blastmaster::database::DatabaseDocument();
    database->setTitle("Database1");

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* objects = new QTableWidget(central);
    objects->setColumnCount(1);
    objects->setHorizontalHeaderLabels(QStringList() << "Database Objects");
    objects->setShowGrid(true);
    objects->setAlternatingRowColors(false);
    objects->setSelectionBehavior(QAbstractItemView::SelectRows);
    objects->setSelectionMode(QAbstractItemView::SingleSelection);
    objects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    objects->verticalHeader()->setVisible(false);
    objects->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(objects);

    window.setCentralWidget(central);

    blastmaster::database::Access95MenuBar menuBar(&window, database);
    window.setMenuBar(menuBar.menuBar());

    auto refreshObjects = [&]() {
        objects->setRowCount(0);
        const auto tables = database->tables();
        objects->setRowCount(tables.size());
        for (int row = 0; row < tables.size(); ++row)
            objects->setItem(row, 0, new QTableWidgetItem(tables.at(row)));
    };

    // Keep the same product/edition status pattern as the other suite apps.
    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Standard);
    const std::string sample_key = "BMSSTD-DATABASE";
    const bool valid = validator.validate(sample_key);
    window.statusBar()->showMessage(QString("Edition: %1 | Valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No"));

    refreshObjects();
    window.show();
    return app.exec();
}
