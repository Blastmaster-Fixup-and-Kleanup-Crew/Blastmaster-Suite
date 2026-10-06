#include <QApplication>
#include <QHeaderView>
#include <QMainWindow>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include "Access95MenuBar.h"
#include "DatabaseDocument.h"

using namespace blastmaster::database;

static void refreshTables(QTableWidget* tableView, DatabaseDocument* document)
{
    tableView->clear();
    const QStringList tables = document->tables();
    tableView->setColumnCount(1);
    tableView->setHorizontalHeaderLabels({QStringLiteral("Database Objects")});
    tableView->setRowCount(tables.size());

    for (int row = 0; row < tables.size(); ++row) {
        tableView->setItem(row, 0, new QTableWidgetItem(tables.at(row)));
    }
    tableView->horizontalHeader()->setStretchLastSection(true);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Blastmaster Database"));
    app.setApplicationDisplayName(QStringLiteral("Blastmaster Database"));

    // Classic Windows/Office-era palette and metrics.
    app.setStyle(QStringLiteral("Windows"));

    QMainWindow window;
    window.resize(1100, 700);
    window.setWindowTitle(QStringLiteral("Blastmaster Database - Database1"));

    auto* document = new DatabaseDocument(&window);
    if (!document->newDatabase()) {
        QMessageBox::critical(&window, QStringLiteral("Database Error"), document->lastError());
        return 1;
    }

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(6, 6, 6, 6);

    auto* objects = new QTableWidget(central);
    objects->setSelectionBehavior(QAbstractItemView::SelectRows);
    objects->setSelectionMode(QAbstractItemView::SingleSelection);
    objects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    objects->setAlternatingRowColors(false);
    objects->setShowGrid(true);
    objects->verticalHeader()->setVisible(false);
    objects->horizontalHeader()->setDefaultSectionSize(180);
    layout->addWidget(objects);

    window.setCentralWidget(central);

    Access95MenuBar accessMenu(&window, document);
    window.setMenuBar(accessMenu.menuBar());

    QObject::connect(document, &DatabaseDocument::databaseChanged, [&] {
        refreshTables(objects, document);
        window.setWindowTitle(QStringLiteral("Blastmaster Database - %1")
            .arg(document->filePath().isEmpty()
                 ? QStringLiteral("Database1")
                 : document->filePath()));
    });

    QObject::connect(document, &DatabaseDocument::errorOccurred, [&](const QString& error) {
        window.statusBar()->showMessage(error, 5000);
    });

    refreshTables(objects, document);
    window.statusBar()->showMessage(QStringLiteral("Ready"), 2000);
    window.show();

    return app.exec();
}
