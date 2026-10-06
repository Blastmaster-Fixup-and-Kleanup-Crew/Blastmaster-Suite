#include <QApplication>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "Excel7MenuBar.h"
#include "Workbook.h"
#include "blastmaster/ProductKeyValidator.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Workbooks");
    app.setApplicationDisplayName("Blastmaster Workbooks");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Workbooks - Book1");

    auto* workbook = new blastmaster::spreadsheet::Workbook();
    workbook->setTitle("Book1");

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* title = new QLabel("Blastmaster Workbooks", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: 600; margin-top: 12px; margin-bottom: 8px;");
    layout->addWidget(title);

    auto* sheet = new QTableWidget(20, 10, central);
    sheet->setShowGrid(true);
    sheet->setAlternatingRowColors(true);
    sheet->horizontalHeader()->setDefaultSectionSize(110);
    sheet->verticalHeader()->setDefaultSectionSize(26);
    sheet->setSelectionMode(QAbstractItemView::ExtendedSelection);
    sheet->setStyleSheet(
        "QTableWidget { background: #ffffff; border: 1px solid #b8b8b8; }"
        "QHeaderView::section { background: #e6e6e6; color: #222; border: 1px solid #b8b8b8; }");

    QStringList columns;
    for (int i = 0; i < 10; ++i) {
        columns << QString(QChar('A' + i));
    }
    sheet->setHorizontalHeaderLabels(columns);

    for (int row = 0; row < sheet->rowCount(); ++row) {
        for (int col = 0; col < sheet->columnCount(); ++col) {
            auto* item = new QTableWidgetItem(QString());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            sheet->setItem(row, col, item);
        }
    }
    layout->addWidget(sheet);

    window.setCentralWidget(central);

    blastmaster::spreadsheet::Excel7MenuBar menuBar(&window, workbook);
    window.setMenuBar(menuBar.menuBar());

    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Standard);
    const std::string sample_key = "BMSSTD-SPREADSHEET";
    const bool valid = validator.validate(sample_key);
    window.statusBar()->showMessage(QString("Edition: %1 | Valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No"));

    window.show();
    return app.exec();
}
