#include "Excel7MenuBar.h"
#include "Workbook.h"
#include "blastmaster/HelpTopicsWindow.h"
#include "blastmaster/RecentFiles.h"

#include <QAction>
#include <QApplication>
#include <QToolBar>
#include <QFontComboBox>
#include <QComboBox>
#include <QKeySequence>
#include <QIcon>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTableWidget>
#include <QTabWidget>
#include <QColorDialog>
#include <QFont>
#include <QInputDialog>
#include <QLineEdit>
#include <functional>

namespace blastmaster::spreadsheet {

static QAction* makeAction(const QString& text, const QKeySequence& seq = QKeySequence(), QObject* parent = nullptr)
{
    QAction* a = new QAction(text, parent);
    if (!seq.isEmpty()) a->setShortcut(seq);
    return a;
}

Excel7MenuBar::Excel7MenuBar(QMainWindow* parent, Workbook* workbook)
    : m_parent(parent)
    , m_workbook(workbook)
{
    m_menuBar = new QMenuBar(parent);

    createFileMenu();
    createEditMenu();
    createViewMenu();
    createInsertMenu();
    createFormatMenu();
    createToolsMenu();
    createDataMenu();
    createWindowMenu();
    createHelpMenu();

    createToolbars();
}

void Excel7MenuBar::createFileMenu()
{
    m_fileMenu = m_menuBar->addMenu("&File");

    QAction* aNew = makeAction("&New", QKeySequence::New, m_parent);
    QAction* aOpen = makeAction("&Open...", QKeySequence::Open, m_parent);
    QAction* aClose = makeAction("C&lose", QKeySequence("Ctrl+W"), m_parent);
    QAction* aSave = makeAction("&Save", QKeySequence::Save, m_parent);
    QAction* aSaveAs = makeAction("Save &As...", QKeySequence::SaveAs, m_parent);
    QAction* aPageSetup = makeAction("Page Setup...", QKeySequence(), m_parent);
    QAction* aPrintArea = makeAction("Print Area", QKeySequence(), m_parent);
    QAction* aPrintPreview = makeAction("Print Preview", QKeySequence("Ctrl+Shift+P"), m_parent);
    QAction* aPrint = makeAction("&Print...", QKeySequence::Print, m_parent);
    QAction* aSend = makeAction("Send", QKeySequence(), m_parent);
    QAction* aProperties = makeAction("Properties", QKeySequence(), m_parent);
    QAction* aExit = makeAction("E&xit", QKeySequence::Quit, m_parent);

    m_fileMenu->addAction(aNew);
    m_fileMenu->addAction(aOpen);
    auto* recentMenu = m_fileMenu->addMenu("Recent Workbooks");
    QObject::connect(recentMenu, &QMenu::aboutToShow, [this, recentMenu]() {
        recentMenu->clear();
        const auto recent = blastmaster::RecentFiles::files(QStringLiteral("workbooks"));
        if (recent.isEmpty()) {
            auto* empty = recentMenu->addAction("(No recent workbooks)");
            empty->setEnabled(false);
            return;
        }
        for (const QString& path : recent) {
            auto* item = recentMenu->addAction(QFileInfo(path).fileName());
            item->setToolTip(path);
            QObject::connect(item, &QAction::triggered, [this, path]() {
                m_workbook->setFilePath(path);
                if (!m_workbook->load()) {
                    if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Failed to open workbook", 3000);
                    return;
                }
                blastmaster::RecentFiles::add(QStringLiteral("workbooks"), path);
                m_parent->setWindowTitle(QString("Blastmaster Workbooks - %1").arg(m_workbook->title()));
                if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook opened: " + path, 3000);
                if (m_documentLoadedCallback) m_documentLoadedCallback();
            });
        }
        recentMenu->addSeparator();
        auto* clear = recentMenu->addAction("Clear Recent Workbooks");
        QObject::connect(clear, &QAction::triggered, [recentMenu]() {
            blastmaster::RecentFiles::clear(QStringLiteral("workbooks"));
            recentMenu->hide();
        });
    });
    m_fileMenu->addAction(aClose);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aSave);
    m_fileMenu->addAction(aSaveAs);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aPageSetup);
    m_fileMenu->addAction(aPrintArea);
    m_fileMenu->addAction(aPrintPreview);
    m_fileMenu->addAction(aPrint);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aSend);
    m_fileMenu->addAction(aProperties);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aExit);

    // basic wiring
    QObject::connect(aOpen, &QAction::triggered, [this]() {
        if (!m_workbook) return;
        QFileDialog dialog(m_parent, "Open Workbook", QString(), Workbook::fileFilter());
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setDefaultSuffix(Workbook::fileExtension().mid(1));
        if (dialog.exec() != QDialog::Accepted || dialog.selectedFiles().isEmpty()) return;
        const QString path = dialog.selectedFiles().first();
        m_workbook->setFilePath(path);
        if (!m_workbook->load()) {
            QMessageBox::critical(m_parent, "Open Workbook", "Failed to open workbook:\n" + path);
            return;
        }
        blastmaster::RecentFiles::add(QStringLiteral("workbooks"), m_workbook->filePath());
        m_parent->setWindowTitle(QString("Blastmaster Workbooks - %1").arg(m_workbook->title()));
        if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook opened: " + path, 3000);
        if (m_documentLoadedCallback) m_documentLoadedCallback();
    });
    QObject::connect(aExit, &QAction::triggered, qApp, &QApplication::quit);

    // Save: if workbook has no file path, prompt Save As, otherwise save
    QObject::connect(aSave, &QAction::triggered, [this]() {
        if (!m_workbook) return;
        if (m_workbook->filePath().isEmpty()) {
            QString path = QFileDialog::getSaveFileName(m_parent, "Save Workbook As", QString(), Workbook::fileFilter());
            if (path.isEmpty()) return;
            if (!path.endsWith(Workbook::fileExtension())) path += Workbook::fileExtension();
            if (m_workbook->saveAs(path)) {
                blastmaster::RecentFiles::add(QStringLiteral("workbooks"), m_workbook->filePath());
                if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook saved", 3000);
            } else {
                QMessageBox::critical(m_parent, QStringLiteral("Save Workbook"),
                    QStringLiteral("Could not save the workbook to:\\n%1").arg(path));
            }
        } else if (m_workbook->save()) {
            blastmaster::RecentFiles::add(QStringLiteral("workbooks"), m_workbook->filePath());
            if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook saved", 3000);
        } else {
            QMessageBox::critical(m_parent, QStringLiteral("Save Workbook"),
                QStringLiteral("Could not save the workbook to:\\n%1").arg(m_workbook->filePath()));
        }
    });

    // Save As: always prompt
    QObject::connect(aSaveAs, &QAction::triggered, [this]() {
        if (!m_workbook) return;
        QString path = QFileDialog::getSaveFileName(m_parent, "Save Workbook As", QString(), Workbook::fileFilter());
        if (path.isEmpty()) return;
        if (!path.endsWith(Workbook::fileExtension())) path += Workbook::fileExtension();
        if (m_workbook->saveAs(path)) {
            blastmaster::RecentFiles::add(QStringLiteral("workbooks"), m_workbook->filePath());
            if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook saved", 3000);
        } else {
            QMessageBox::critical(m_parent, QStringLiteral("Save Workbook"),
                QStringLiteral("Could not save the workbook to:\\n%1").arg(path));
        }
    });
}

void Excel7MenuBar::createEditMenu()
{
    m_editMenu = m_menuBar->addMenu("&Edit");
    m_editMenu->addAction(makeAction("Undo", QKeySequence::Undo, m_parent));
    m_editMenu->addAction(makeAction("Repeat", QKeySequence::Redo, m_parent));
    m_editMenu->addSeparator();
    m_editMenu->addAction(makeAction("Cut", QKeySequence::Cut, m_parent));
    m_editMenu->addAction(makeAction("Copy", QKeySequence::Copy, m_parent));
    m_editMenu->addAction(makeAction("Paste", QKeySequence::Paste, m_parent));
    m_editMenu->addAction(makeAction("Paste Special...", QKeySequence(), m_parent));
    m_editMenu->addSeparator();
    auto* find = makeAction("Find...", QKeySequence::Find, m_parent);
    m_editMenu->addAction(find);
    QObject::connect(find, &QAction::triggered, [this]() {
        auto* tabs = m_parent->findChild<QTabWidget*>();
        auto* table = tabs ? qobject_cast<QTableWidget*>(tabs->currentWidget()) : nullptr;
        if (!table) return;
        bool ok = false;
        const QString needle = QInputDialog::getText(m_parent, QStringLiteral("Find"),
            QStringLiteral("Find what:"), QLineEdit::Normal, QString(), &ok);
        if (!ok || needle.isEmpty()) return;
        const int start = table->currentRow() * table->columnCount() + table->currentColumn() + 1;
        const int total = table->rowCount() * table->columnCount();
        for (int offset = 0; offset < total; ++offset) {
            const int flat = (start + offset) % total;
            const int row = flat / table->columnCount();
            const int col = flat % table->columnCount();
            auto* item = table->item(row, col);
            if (item && item->text().contains(needle, Qt::CaseInsensitive)) {
                table->setCurrentCell(row, col);
                table->scrollToItem(item);
                m_parent->statusBar()->showMessage(QStringLiteral("Found %1 at %2%3")
                    .arg(needle, QString(QChar('A' + col)), QString::number(row + 1)), 2500);
                return;
            }
        }
        QMessageBox::information(m_parent, QStringLiteral("Find"), QStringLiteral("No matching cells found."));
    });
    auto* replace = makeAction("Replace...", QKeySequence::Replace, m_parent);
    m_editMenu->addAction(replace);
    QObject::connect(replace, &QAction::triggered, [this]() {
        auto* tabs = m_parent->findChild<QTabWidget*>();
        auto* table = tabs ? qobject_cast<QTableWidget*>(tabs->currentWidget()) : nullptr;
        if (!table) return;
        bool ok = false;
        const QString needle = QInputDialog::getText(m_parent, QStringLiteral("Replace"),
            QStringLiteral("Find what:"), QLineEdit::Normal, QString(), &ok);
        if (!ok || needle.isEmpty()) return;
        const QString replacement = QInputDialog::getText(m_parent, QStringLiteral("Replace"),
            QStringLiteral("Replace with:"), QLineEdit::Normal, QString(), &ok);
        if (!ok) return;
        int count = 0;
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 0; col < table->columnCount(); ++col) {
                auto* item = table->item(row, col);
                if (item && item->text().contains(needle, Qt::CaseSensitive)) {
                    item->setText(QString(item->text()).replace(needle, replacement));
                    ++count;
                }
            }
        }
        m_parent->statusBar()->showMessage(QStringLiteral("Replaced %1 cell value(s).").arg(count), 3000);
    });
    m_editMenu->addAction(makeAction("Go To...", QKeySequence("Ctrl+G"), m_parent));
}

void Excel7MenuBar::createViewMenu()
{
    m_viewMenu = m_menuBar->addMenu("&View");
    m_viewMenu->addAction(makeAction("Normal", QKeySequence(), m_parent));
    m_viewMenu->addAction(makeAction("Page Break Preview", QKeySequence(), m_parent));
    m_viewMenu->addSeparator();
    auto* toolbars = makeAction("Toolbars...", QKeySequence(), m_parent);
    m_viewMenu->addAction(toolbars);
    QObject::connect(toolbars, &QAction::triggered, [this]() {
        const auto bars = m_parent->findChildren<QToolBar*>();
        bool anyVisible = false;
        for (QToolBar* toolbar : bars) anyVisible = anyVisible || toolbar->isVisible();
        for (QToolBar* toolbar : bars) toolbar->setVisible(!anyVisible);
    });
    auto* formulaBar = makeAction("Formula Bar", QKeySequence(), m_parent);
    formulaBar->setCheckable(true);
    formulaBar->setChecked(true);
    m_viewMenu->addAction(formulaBar);
    QObject::connect(formulaBar, &QAction::toggled, [this](bool visible) {
        if (auto* editor = m_parent->findChild<QLineEdit*>(QStringLiteral("formulaBar")))
            editor->setVisible(visible);
    });
    auto* status = makeAction("Status Bar", QKeySequence(), m_parent);
    status->setCheckable(true);
    status->setChecked(true);
    m_viewMenu->addAction(status);
    QObject::connect(status, &QAction::toggled, [this](bool visible) {
        if (m_parent->statusBar()) m_parent->statusBar()->setVisible(visible);
    });
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(makeAction("Header and Footer...", QKeySequence(), m_parent));
    m_viewMenu->addAction(makeAction("Zoom...", QKeySequence(), m_parent));
}

void Excel7MenuBar::createInsertMenu()
{
    m_insertMenu = m_menuBar->addMenu("&Insert");
    m_insertMenu->addAction(makeAction("Cells...", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Rows", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Columns", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Worksheet", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Chart...", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Function...", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Picture...", QKeySequence(), m_parent));
    m_insertMenu->addAction(makeAction("Object...", QKeySequence(), m_parent));
}

void Excel7MenuBar::createFormatMenu()
{
    m_formatMenu = m_menuBar->addMenu("F&ormat");
    m_formatMenu->addAction(makeAction("Cells...", QKeySequence(), m_parent));
    m_formatMenu->addAction(makeAction("Row", QKeySequence(), m_parent));
    m_formatMenu->addAction(makeAction("Column", QKeySequence(), m_parent));
    m_formatMenu->addAction(makeAction("Sheet", QKeySequence(), m_parent));
    m_formatMenu->addAction(makeAction("AutoFormat...", QKeySequence(), m_parent));
    m_formatMenu->addAction(makeAction("Conditional Formatting...", QKeySequence(), m_parent));
    m_formatMenu->addAction(makeAction("Style...", QKeySequence(), m_parent));
}

void Excel7MenuBar::createToolsMenu()
{
    m_toolsMenu = m_menuBar->addMenu("&Tools");
    m_toolsMenu->addAction(makeAction("Spelling...", QKeySequence(), m_parent));
    m_toolsMenu->addAction(makeAction("AutoCorrect...", QKeySequence(), m_parent));
    m_toolsMenu->addAction(makeAction("Trace Precedents", QKeySequence(), m_parent));
    m_toolsMenu->addAction(makeAction("Goal Seek...", QKeySequence(), m_parent));
    m_toolsMenu->addAction(makeAction("Scenarios...", QKeySequence(), m_parent));
    m_toolsMenu->addAction(makeAction("Options...", QKeySequence(), m_parent));
}

void Excel7MenuBar::createDataMenu()
{
    m_dataMenu = m_menuBar->addMenu("&Data");
    auto* sort = makeAction("Sort...", QKeySequence(), m_parent);
    m_dataMenu->addAction(sort);
    QObject::connect(sort, &QAction::triggered, [this]() {
        auto* tabs = m_parent->findChild<QTabWidget*>();
        auto* table = tabs ? qobject_cast<QTableWidget*>(tabs->currentWidget()) : nullptr;
        if (!table || table->columnCount() == 0) return;
        QStringList columns;
        for (int col = 0; col < table->columnCount(); ++col)
            columns << QString(QChar('A' + col));
        bool ok = false;
        const QString column = QInputDialog::getItem(m_parent, QStringLiteral("Sort"),
            QStringLiteral("Sort by column:"), columns, qMax(0, table->currentColumn()), false, &ok);
        if (!ok) return;
        const auto order = QMessageBox::question(m_parent, QStringLiteral("Sort Order"),
            QStringLiteral("Sort ascending? Choose No for descending."),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes
            ? Qt::AscendingOrder : Qt::DescendingOrder;
        table->setSortingEnabled(true);
        table->sortItems(columns.indexOf(column), order);
        table->setSortingEnabled(false);
        m_parent->statusBar()->showMessage(QStringLiteral("Sorted by column %1").arg(column), 2500);
    });
    auto* filter = makeAction("Filter...", QKeySequence(), m_parent);
    m_dataMenu->addAction(filter);
    QObject::connect(filter, &QAction::triggered, [this]() {
        auto* tabs = m_parent->findChild<QTabWidget*>();
        auto* table = tabs ? qobject_cast<QTableWidget*>(tabs->currentWidget()) : nullptr;
        if (!table || table->columnCount() == 0) return;
        bool ok = false;
        const QString value = QInputDialog::getText(m_parent, QStringLiteral("Filter"),
            QStringLiteral("Show rows where the selected column contains (leave blank to clear):"),
            QLineEdit::Normal, QString(), &ok);
        if (!ok) return;
        const int column = qMax(0, table->currentColumn());
        for (int row = 0; row < table->rowCount(); ++row) {
            auto* item = table->item(row, column);
            const bool visible = value.isEmpty() || (item && item->text().contains(value, Qt::CaseInsensitive));
            table->setRowHidden(row, !visible);
        }
        m_parent->statusBar()->showMessage(value.isEmpty()
            ? QStringLiteral("Filter cleared.")
            : QStringLiteral("Filter applied to column %1.").arg(QChar('A' + column)), 2500);
    });
    m_dataMenu->addAction(makeAction("Form...", QKeySequence(), m_parent));
    m_dataMenu->addAction(makeAction("Subtotals...", QKeySequence(), m_parent));
    m_dataMenu->addAction(makeAction("Text to Columns...", QKeySequence(), m_parent));
    m_dataMenu->addAction(makeAction("PivotTable...", QKeySequence(), m_parent));
    m_dataMenu->addAction(makeAction("Group and Outline", QKeySequence(), m_parent));
}

void Excel7MenuBar::createWindowMenu()
{
    m_windowMenu = m_menuBar->addMenu("&Window");
    m_windowMenu->addAction(makeAction("New Window", QKeySequence(), m_parent));
    m_windowMenu->addAction(makeAction("Arrange...", QKeySequence(), m_parent));
    m_windowMenu->addAction(makeAction("Hide", QKeySequence(), m_parent));
    m_windowMenu->addAction(makeAction("Unhide...", QKeySequence(), m_parent));
    m_windowMenu->addAction(makeAction("Split", QKeySequence(), m_parent));
    m_windowMenu->addAction(makeAction("Freeze Panes", QKeySequence(), m_parent));
    m_windowMenu->addSeparator();
    QAction* placeholder = makeAction("(No open workbooks)", QKeySequence(), m_parent);
    placeholder->setEnabled(false);
    m_windowMenu->addAction(placeholder);
}

void Excel7MenuBar::createHelpMenu()
{
    m_helpMenu = m_menuBar->addMenu("&Help");
    auto* helpTopics = makeAction("Microsoft Excel Help Topics", QKeySequence::HelpContents, m_parent);
    m_helpMenu->addAction(helpTopics);
    QObject::connect(helpTopics, &QAction::triggered, [this]() {
        blastmaster::HelpTopicsWindow::showFor(m_parent, "workbooks");
    });
    m_helpMenu->addAction(makeAction("What's This?", QKeySequence::WhatsThis, m_parent));
    m_helpMenu->addAction(makeAction("Tip of the Day", QKeySequence(), m_parent));
    auto* about = makeAction("About Microsoft Excel", QKeySequence(), m_parent);
    m_helpMenu->addAction(about);
    QObject::connect(about, &QAction::triggered, [this]() {
        QMessageBox::about(m_parent, "About Blastmaster Workbooks", "Blastmaster Workbooks\nVersion 1.0.0\nClassic office-style spreadsheet application.");
    });
}

void Excel7MenuBar::createToolbars()
{
    // Standard toolbar
    QToolBar* standard = new QToolBar("Standard", m_parent);
    standard->setObjectName("toolbar_standard");
    standard->setMovable(true);
    standard->addAction(makeAction("New", QKeySequence::New, m_parent));
    standard->addAction(makeAction("Open", QKeySequence::Open, m_parent));
    standard->addAction(makeAction("Save", QKeySequence::Save, m_parent));
    standard->addSeparator();
    standard->addAction(makeAction("Cut", QKeySequence::Cut, m_parent));
    standard->addAction(makeAction("Copy", QKeySequence::Copy, m_parent));
    standard->addAction(makeAction("Paste", QKeySequence::Paste, m_parent));
    standard->addSeparator();
    standard->addAction(makeAction("Undo", QKeySequence::Undo, m_parent));
    standard->addAction(makeAction("Redo", QKeySequence::Redo, m_parent));
    m_parent->addToolBar(Qt::TopToolBarArea, standard);

    // Formatting toolbar
    QToolBar* formatting = new QToolBar("Formatting", m_parent);
    formatting->setObjectName("toolbar_formatting");
    formatting->setMovable(true);

    auto* fontCombo = new QFontComboBox(formatting);
    formatting->addWidget(fontCombo);

    auto* sizeCombo = new QComboBox(formatting);
    const QList<int> sizes = {8,9,10,11,12,14,16,18,20,22,24,26,28,36};
    for (int pointSize : sizes) sizeCombo->addItem(QString::number(pointSize));
    sizeCombo->setCurrentText("10");
    formatting->addWidget(sizeCombo);

    auto selectedCells = [this]() {
        QList<QTableWidgetItem*> items;
        auto* tabs = m_parent->findChild<QTabWidget*>();
        auto* table = tabs ? qobject_cast<QTableWidget*>(tabs->currentWidget()) : nullptr;
        if (!table) return items;
        items = table->selectedItems();
        if (items.isEmpty() && table->currentItem()) items.append(table->currentItem());
        return items;
    };
    auto applyFont = [selectedCells](const std::function<void(QFont&)>& update) {
        const auto items = selectedCells();
        for (auto* item : items) {
            QFont font = item->font();
            update(font);
            item->setFont(font);
        }
    };

    QObject::connect(fontCombo, &QFontComboBox::currentFontChanged, [this, applyFont](const QFont& chosen) {
        applyFont([&chosen](QFont& font) { font.setFamily(chosen.family()); });
        if (m_parent->statusBar()) m_parent->statusBar()->showMessage(QString("Font: %1").arg(chosen.family()), 2500);
    });
    QObject::connect(sizeCombo, &QComboBox::currentTextChanged, [this, applyFont](const QString& text) {
        bool ok = false;
        const int pointSize = text.toInt(&ok);
        if (ok) applyFont([pointSize](QFont& font) { font.setPointSize(pointSize); });
        if (m_parent->statusBar()) m_parent->statusBar()->showMessage(QString("Font size: %1").arg(text), 2500);
    });

    auto addFormatAction = [this, formatting, selectedCells](const QString& text, const QKeySequence& shortcut = QKeySequence()) {
        auto* action = makeAction(text, shortcut, m_parent);
        formatting->addAction(action);
        QObject::connect(action, &QAction::triggered, [this, text, selectedCells]() {
            const auto items = selectedCells();
            for (auto* item : items) {
                QFont font = item->font();
                if (text == "Bold") font.setBold(!font.bold());
                else if (text == "Italic") font.setItalic(!font.italic());
                else if (text == "Underline") font.setUnderline(!font.underline());
                else if (text == "Align Left") item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                else if (text == "Center") item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                else if (text == "Align Right") item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                else if (text == "Font Color") {
                    const QColor color = QColorDialog::getColor(item->foreground().color(), m_parent, "Cell Font Color");
                    if (color.isValid()) item->setForeground(color);
                }
                if (text == "Bold" || text == "Italic" || text == "Underline") item->setFont(font);
            }
            if (items.isEmpty() && m_parent->statusBar())
                m_parent->statusBar()->showMessage("Select one or more cells first.", 2500);
        });
    };
    addFormatAction("Bold", QKeySequence::Bold);
    addFormatAction("Italic", QKeySequence::Italic);
    addFormatAction("Underline");
    addFormatAction("Align Left");
    addFormatAction("Center");
    addFormatAction("Align Right");
    addFormatAction("Font Color");

    m_parent->addToolBar(Qt::TopToolBarArea, formatting);
}

} // namespace blastmaster::spreadsheet
