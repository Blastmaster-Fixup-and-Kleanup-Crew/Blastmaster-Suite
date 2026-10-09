#include <QApplication>
#include <QHeaderView>
#include <QInputDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

#include "Access95MenuBar.h"
#include "DatabaseDocument.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Databases");
    app.setApplicationDisplayName("Blastmaster Databases");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Blastmaster");
    app.setOrganizationDomain("blastmaster.local");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Databases - Database1");
    window.setAccessibleName("Blastmaster Databases");

    auto* database = new blastmaster::database::DatabaseDocument(&window);
    database->newDatabase();

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    auto* controls = new QHBoxLayout();
    auto* tableSelector = new QComboBox(central);
    tableSelector->setAccessibleName("Table selector");
    auto* newTable = new QPushButton("New Table", central);
    auto* deleteTable = new QPushButton("Delete Table", central);
    auto* addRecord = new QPushButton("Add Record", central);
    auto* deleteRecord = new QPushButton("Delete Record", central);
    controls->addWidget(tableSelector, 1);
    controls->addWidget(newTable);
    controls->addWidget(deleteTable);
    controls->addWidget(addRecord);
    controls->addWidget(deleteRecord);
    layout->addLayout(controls);

    auto* grid = new QTableWidget(central);
    grid->setAccessibleName("Database record editor");
    grid->setSelectionBehavior(QAbstractItemView::SelectRows);
    grid->setSelectionMode(QAbstractItemView::SingleSelection);
    // Typing with a cell selected should enter edit mode immediately.
    grid->setEditTriggers(QAbstractItemView::AnyKeyPressed |
                          QAbstractItemView::DoubleClicked |
                          QAbstractItemView::EditKeyPressed |
                          QAbstractItemView::SelectedClicked);
    grid->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(grid);
    window.setCentralWidget(central);

    blastmaster::database::Access95MenuBar menuBar(&window, database);
    window.setMenuBar(menuBar.menuBar());

    bool refreshing = false;
    auto refreshTables = [&]() {
        const QString current = tableSelector->currentText();
        tableSelector->clear();
        tableSelector->addItems(database->tables());
        const int index = tableSelector->findText(current);
        if (index >= 0) tableSelector->setCurrentIndex(index);
    };

    auto refreshRecords = [&]() {
        const QString table = tableSelector->currentText();
        if (table.isEmpty()) {
            grid->clear();
            grid->setRowCount(0);
            return;
        }
        refreshing = true;
        const QStringList columns = database->columns(table);
        const auto rows = database->records(table);
        grid->clear();
        grid->setColumnCount(columns.size());
        grid->setHorizontalHeaderLabels(columns);
        grid->setRowCount(rows.size());
        for (int row = 0; row < rows.size(); ++row)
            for (int col = 0; col < columns.size(); ++col)
                grid->setItem(row, col, new QTableWidgetItem(rows.at(row).value(col).toString()));
        refreshing = false;
    };

    QObject::connect(tableSelector, &QComboBox::currentTextChanged, [&](const QString&) {
        refreshRecords();
    });
    QObject::connect(newTable, &QPushButton::clicked, [&]() {
        bool ok = false;
        const QString name = QInputDialog::getText(&window, "New Table", "Table name:", QLineEdit::Normal, "Table1", &ok);
        if (ok && !name.trimmed().isEmpty() && database->createTable(name.trimmed())) {
            refreshTables();
            tableSelector->setCurrentText(name.trimmed());
        }
    });
    QObject::connect(deleteTable, &QPushButton::clicked, [&]() {
        const QString table = tableSelector->currentText();
        if (!table.isEmpty() && database->deleteTable(table))
            refreshTables();
    });
    QObject::connect(addRecord, &QPushButton::clicked, [&]() {
        const QString table = tableSelector->currentText();
        if (table.isEmpty()) return;
        const QStringList columns = database->columns(table);
        QVariantMap values;
        for (const QString& column : columns)
            if (column.compare("ID", Qt::CaseInsensitive) != 0)
                values.insert(column, QString());
        if (database->insertRecord(table, values)) refreshRecords();
    });
    QObject::connect(deleteRecord, &QPushButton::clicked, [&]() {
        const QString table = tableSelector->currentText();
        const int row = grid->currentRow();
        if (table.isEmpty() || row < 0) return;
        const auto rows = database->records(table);
        if (row >= rows.size()) return;
        const int id = rows.at(row).value(0).toInt();
        if (database->deleteRecord(table, id)) refreshRecords();
    });
    QObject::connect(grid, &QTableWidget::cellChanged, [&](int row, int column) {
        if (refreshing) return;
        const QString table = tableSelector->currentText();
        const auto rows = database->records(table);
        const QStringList columns = database->columns(table);
        if (row < 0 || row >= rows.size() || column <= 0 || column >= columns.size()) return;
        QVariantMap values;
        values.insert(columns.at(column), grid->item(row, column) ? grid->item(row, column)->text() : QString());
        database->updateRecord(table, rows.at(row).value(0).toInt(), values);
    });
    QObject::connect(database, &blastmaster::database::DatabaseDocument::databaseChanged, [&]() {
        refreshTables();
        refreshRecords();
    });

    refreshTables();
    refreshRecords();
    window.statusBar()->showMessage("Ready");
    window.show();
    tableSelector->setFocus();
    return app.exec();
}
