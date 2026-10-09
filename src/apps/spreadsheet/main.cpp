#include <QApplication>
#include <QHeaderView>
#include <QMainWindow>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>

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

    auto* controls = new QHBoxLayout();
    auto* addSheet = new QPushButton("New Sheet", central);
    auto* deleteSheet = new QPushButton("Delete Sheet", central);
    auto* formulaBar = new QLineEdit(central);
    formulaBar->setPlaceholderText("Formula / value");
    formulaBar->setAccessibleName("Formula bar");
    controls->addWidget(addSheet);
    controls->addWidget(deleteSheet);
    controls->addWidget(formulaBar, 1);
    layout->addLayout(controls);

    auto* tabs = new QTabWidget(central);
    tabs->setAccessibleName("Worksheets");
    layout->addWidget(tabs);

    auto makeSheet = [&](const QString& name) {
        auto* view = new QTableWidget(100, 26, tabs);
        view->setAccessibleName(name + " worksheet");
        // Start editing as soon as the user types while a cell is selected.
        // Double-click and F2/EditKeyPressed remain available as alternatives.
        view->setEditTriggers(QAbstractItemView::AnyKeyPressed |
                              QAbstractItemView::DoubleClicked |
                              QAbstractItemView::EditKeyPressed |
                              QAbstractItemView::SelectedClicked);
        view->setSelectionMode(QAbstractItemView::ExtendedSelection);
        view->setSelectionBehavior(QAbstractItemView::SelectItems);
        view->horizontalHeader()->setDefaultSectionSize(90);
        view->verticalHeader()->setDefaultSectionSize(22);
        QStringList columns;
        for (int i = 0; i < 26; ++i) columns << QString(QChar('A' + i));
        view->setHorizontalHeaderLabels(columns);
        for (int row = 0; row < view->rowCount(); ++row)
            for (int col = 0; col < view->columnCount(); ++col) {
                const QString address = QString(QChar('A' + col)) + QString::number(row + 1);
                view->setItem(row, col, new QTableWidgetItem(workbook->cell(name, address)));
            }
        QObject::connect(view, &QTableWidget::cellChanged, [workbook, name, view](int row, int col) {
            if (auto* item = view->item(row, col))
                workbook->setCell(name, QString(QChar('A' + col)) + QString::number(row + 1), item->text());
        });
        QObject::connect(view, &QTableWidget::itemSelectionChanged, [&window, workbook, tabs, formulaBar, view, name]() {
            const auto ranges = view->selectedRanges();
            if (ranges.isEmpty()) return;
            const auto range = ranges.first();
            const QString address = QString(QChar('A' + range.leftColumn())) + QString::number(range.topRow() + 1);
            formulaBar->setText(workbook->cell(name, address));
            window.statusBar()->showMessage(QString("Cell %1 | Result: %2").arg(address, workbook->evaluateCell(name, address)));
        });
        return view;
    };

    auto refreshWorkbookUi = [&]() {
        while (tabs->count() > 0)
            delete tabs->widget(0);
        for (const QString& name : workbook->sheets())
            tabs->addTab(makeSheet(name), name);
        window.setWindowTitle(QString("Blastmaster Workbooks - %1").arg(workbook->title()));
    };
    refreshWorkbookUi();

    QObject::connect(addSheet, &QPushButton::clicked, [&]() {
        const QString name = QString("Sheet%1").arg(workbook->sheets().size() + 1);
        if (workbook->addSheet(name)) {
            tabs->addTab(makeSheet(name), name);
            tabs->setCurrentIndex(tabs->count() - 1);
        }
    });
    QObject::connect(deleteSheet, &QPushButton::clicked, [&]() {
        if (tabs->count() <= 1) return;
        const int index = tabs->currentIndex();
        const QString name = tabs->tabText(index);
        if (workbook->removeSheet(name)) {
            delete tabs->widget(index);
            window.statusBar()->showMessage("Worksheet deleted", 2000);
        }
    });
    QObject::connect(formulaBar, &QLineEdit::returnPressed, [&]() {
        auto* view = qobject_cast<QTableWidget*>(tabs->currentWidget());
        if (!view || !view->currentItem()) return;
        view->currentItem()->setText(formulaBar->text());
    });

    window.setCentralWidget(central);
    blastmaster::spreadsheet::Excel7MenuBar menuBar(&window, workbook);
    menuBar.setDocumentLoadedCallback([&]() {
        refreshWorkbookUi();
        if (auto* view = qobject_cast<QTableWidget*>(tabs->currentWidget()))
            view->setFocus();
    });
    window.setMenuBar(menuBar.menuBar());
    window.statusBar()->showMessage("Ready");
    window.show();
    if (auto* view = qobject_cast<QTableWidget*>(tabs->currentWidget()))
        view->setFocus();
    return app.exec();
}
