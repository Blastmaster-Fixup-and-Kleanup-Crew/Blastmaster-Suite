#include "Excel7MenuBar.h"
#include "Workbook.h"
#include "blastmaster/HelpTopicsWindow.h"

#include <QAction>
#include <QToolBar>
#include <QFontComboBox>
#include <QComboBox>
#include <QKeySequence>
#include <QIcon>
#include <QStatusBar>
#include <QFileDialog>

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
    QObject::connect(aExit, &QAction::triggered, qApp, &QApplication::quit);

    // Save: if workbook has no file path, prompt Save As, otherwise save
    QObject::connect(aSave, &QAction::triggered, [this]() {
        if (!m_workbook) return;
        if (m_workbook->filePath().isEmpty()) {
            QString path = QFileDialog::getSaveFileName(m_parent, "Save Workbook As", QString(), Workbook::fileFilter());
            if (path.isEmpty()) return;
            if (!path.endsWith(Workbook::fileExtension())) path += Workbook::fileExtension();
            m_workbook->setFilePath(path);
            m_workbook->save();
            if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook saved", 3000);
        } else {
            m_workbook->save();
            if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook saved", 3000);
        }
    });

    // Save As: always prompt
    QObject::connect(aSaveAs, &QAction::triggered, [this]() {
        if (!m_workbook) return;
        QString path = QFileDialog::getSaveFileName(m_parent, "Save Workbook As", QString(), Workbook::fileFilter());
        if (path.isEmpty()) return;
        if (!path.endsWith(Workbook::fileExtension())) path += Workbook::fileExtension();
        m_workbook->setFilePath(path);
        m_workbook->save();
        if (m_parent->statusBar()) m_parent->statusBar()->showMessage("Workbook saved", 3000);
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
    m_editMenu->addAction(makeAction("Find...", QKeySequence::Find, m_parent));
    m_editMenu->addAction(makeAction("Replace...", QKeySequence::Replace, m_parent));
    m_editMenu->addAction(makeAction("Go To...", QKeySequence("Ctrl+G"), m_parent));
}

void Excel7MenuBar::createViewMenu()
{
    m_viewMenu = m_menuBar->addMenu("&View");
    m_viewMenu->addAction(makeAction("Normal", QKeySequence(), m_parent));
    m_viewMenu->addAction(makeAction("Page Break Preview", QKeySequence(), m_parent));
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(makeAction("Toolbars...", QKeySequence(), m_parent));
    m_viewMenu->addAction(makeAction("Formula Bar", QKeySequence(), m_parent));
    m_viewMenu->addAction(makeAction("Status Bar", QKeySequence(), m_parent));
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
    m_dataMenu->addAction(makeAction("Sort...", QKeySequence(), m_parent));
    m_dataMenu->addAction(makeAction("Filter...", QKeySequence(), m_parent));
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
    m_helpMenu->addAction(makeAction("About Microsoft Excel", QKeySequence(), m_parent));
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
    for (int s : sizes) sizeCombo->addItem(QString::number(s));
    sizeCombo->setCurrentText("10");
    formatting->addWidget(sizeCombo);

    formatting->addAction(makeAction("Bold", QKeySequence::Bold, m_parent));
    formatting->addAction(makeAction("Italic", QKeySequence::Italic, m_parent));
    formatting->addAction(makeAction("Underline", QKeySequence(), m_parent));

    m_parent->addToolBar(Qt::TopToolBarArea, formatting);

    // Connect some demo signals to status bar
    QObject::connect(fontCombo, &QFontComboBox::currentFontChanged, [this](const QFont& f){
        if (m_parent->statusBar()) m_parent->statusBar()->showMessage(QString("Font: %1").arg(f.family()), 2500);
    });
    QObject::connect(sizeCombo, &QComboBox::currentTextChanged, [this](const QString& s){
        if (m_parent->statusBar()) m_parent->statusBar()->showMessage(QString("Font size: %1").arg(s), 2500);
    });
}

} // namespace blastmaster::spreadsheet
