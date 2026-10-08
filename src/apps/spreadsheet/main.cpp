#include <QApplication>
#include <QHeaderView>
#include <QMainWindow>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include "Excel7MenuBar.h"
#include "Workbook.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Workbooks");
    app.setApplicationDisplayName("Blastmaster Workbooks");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Blastmaster");
    app.setOrganizationDomain("blastmaster.local");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Workbooks - Book1");
    window.setAccessibleName("Blastmaster Workbooks");

    auto* workbook = new blastmaster::spreadsheet::Workbook();

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* sheet = new QTableWidget(50, 26, central);
    sheet->setObjectName("workbookGrid");
    sheet->setAccessibleName("Workbook grid");
    sheet->setAccessibleDescription("Editable spreadsheet grid. Use Tab to move across cells and Enter to edit the selected cell.");
    sheet->setToolTip("Workbook grid");
    sheet->setShowGrid(true);
    sheet->setAlternatingRowColors(false);
    sheet->setEditTriggers(QAbstractItemView::DoubleClicked |
                           QAbstractItemView::EditKeyPressed |
                           QAbstractItemView::SelectedClicked);
    sheet->setSelectionMode(QAbstractItemView::ExtendedSelection);
    sheet->setSelectionBehavior(QAbstractItemView::SelectItems);
    sheet->horizontalHeader()->setDefaultSectionSize(90);
    sheet->verticalHeader()->setDefaultSectionSize(22);

    QStringList columns;
    for (int i = 0; i < 26; ++i)
        columns << QString(QChar('A' + i));
    sheet->setHorizontalHeaderLabels(columns);

    for (int row = 0; row < sheet->rowCount(); ++row) {
        for (int col = 0; col < sheet->columnCount(); ++col) {
            const QString address = QString(QChar('A' + col)) + QString::number(row + 1);
            sheet->setItem(row, col, new QTableWidgetItem(workbook->cell("Sheet1", address)));
        }
    }

    QObject::connect(sheet, &QTableWidget::cellChanged,
        [workbook, sheet](int row, int column) {
            auto* item = sheet->item(row, column);
            if (!item) return;
            const QString address = QString(QChar('A' + column)) + QString::number(row + 1);
            workbook->setCell("Sheet1", address, item->text());
        });

    layout->addWidget(sheet);
    window.setCentralWidget(central);

    blastmaster::spreadsheet::Excel7MenuBar menuBar(&window, workbook);
    window.setMenuBar(menuBar.menuBar());

    QObject::connect(sheet, &QTableWidget::itemSelectionChanged, [&window, sheet]() {
        const auto ranges = sheet->selectedRanges();
        if (!ranges.isEmpty()) {
            const auto r = ranges.first();
            window.statusBar()->showMessage(
                QString("Cell %1%2").arg(QChar('A' + r.leftColumn())).arg(r.topRow() + 1));
        }
    });

    window.statusBar()->showMessage("Ready");
    window.show();
    sheet->setFocus();
    return app.exec();
}
