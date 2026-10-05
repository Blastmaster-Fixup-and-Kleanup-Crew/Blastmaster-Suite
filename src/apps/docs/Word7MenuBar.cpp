#include "Word7MenuBar.h"

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QToolBar>

namespace blastmaster::docs {

namespace {
QAction* addMenuAction(QMenu* menu, const QString& text, const QString& statusTip = QString())
{
    auto* action = new QAction(text, menu);
    if (!statusTip.isEmpty()) {
        action->setStatusTip(statusTip);
    }
    menu->addAction(action);
    return action;
}

QAction* addToolbarAction(QToolBar* toolbar, const QString& text, const QString& statusTip = QString())
{
    auto* action = new QAction(text, toolbar);
    if (!statusTip.isEmpty()) {
        action->setStatusTip(statusTip);
    }
    toolbar->addAction(action);
    return action;
}
} // namespace

Word7MenuBar::Word7MenuBar(QMainWindow* parent)
    : m_parent(parent)
    , m_menuBar(new QMenuBar(parent))
    , m_fileMenu(nullptr)
    , m_editMenu(nullptr)
    , m_viewMenu(nullptr)
    , m_insertMenu(nullptr)
    , m_formatMenu(nullptr)
    , m_toolsMenu(nullptr)
    , m_tableMenu(nullptr)
    , m_windowMenu(nullptr)
    , m_helpMenu(nullptr)
{
    parent->setMenuBar(m_menuBar);

    if (!parent->statusBar()) {
        parent->setStatusBar(new QStatusBar(parent));
    }

    createFileMenu();
    createEditMenu();
    createViewMenu();
    createInsertMenu();
    createFormatMenu();
    createToolsMenu();
    createTableMenu();
    createWindowMenu();
    createHelpMenu();
    createToolbars();
    createRuler();

    parent->statusBar()->showMessage("Word processor initialized");
}

void Word7MenuBar::wireAction(QAction* action, const QString& label)
{
    QObject::connect(action, &QAction::triggered, [this, label]() {
        if (m_parent && m_parent->statusBar()) {
            m_parent->statusBar()->showMessage(label + " selected");
        }
    });
}

void Word7MenuBar::createFileMenu()
{
    m_fileMenu = new QMenu("&File", m_parent);
    m_menuBar->addMenu(m_fileMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_fileMenu, text, text + " command");
        wireAction(action, text);
    };

    add("&New");
    add("&Open");
    add("&Close");
    add("&Save");
    add("Save &As");
    add("&Version");
    add("Find File");
    add("Page Setup");
    add("Print Preview");
    add("Print");
    add("Send");
    add("Properties");
    add("E&xit");
}

void Word7MenuBar::createEditMenu()
{
    m_editMenu = new QMenu("&Edit", m_parent);
    m_menuBar->addMenu(m_editMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_editMenu, text, text + " command");
        wireAction(action, text);
    };

    add("&Undo");
    add("&Repeat");
    add("Cu&t");
    add("&Copy");
    add("&Paste");
    add("Paste Special");
    add("Clear");
    add("Select All");
    add("Find");
    add("Replace");
    add("Go To");
    add("AutoText");
}

void Word7MenuBar::createViewMenu()
{
    m_viewMenu = new QMenu("&View", m_parent);
    m_menuBar->addMenu(m_viewMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_viewMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Normal");
    add("Page Layout");
    add("Outline");
    add("Master Document");
    add("Toolbars");
    add("Ruler");
    add("Header and Footer");
    add("Footnotes");
    add("Full Screen");
    add("Zoom");
}

void Word7MenuBar::createInsertMenu()
{
    m_insertMenu = new QMenu("&Insert", m_parent);
    m_menuBar->addMenu(m_insertMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_insertMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Break");
    add("Page Numbers");
    add("Date and Time");
    add("AutoText");
    add("Field");
    add("Symbol");
    add("Form Field");
    add("Comment");
    add("Footnote");
    add("Caption");
    add("Cross-reference");
    add("Index and Tables");
    add("Picture");
    add("Object");
    add("From File");
}

void Word7MenuBar::createFormatMenu()
{
    m_formatMenu = new QMenu("&Format", m_parent);
    m_menuBar->addMenu(m_formatMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_formatMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Font");
    add("Paragraph");
    add("Tabs");
    add("Borders and Shading");
    add("Columns");
    add("Drop Cap");
    add("Bullets and Numbering");
    add("Change Case");
    add("Heading Numbering");
    add("Style");
    add("AutoFormat");
    add("Theme");
}

void Word7MenuBar::createToolsMenu()
{
    m_toolsMenu = new QMenu("&Tools", m_parent);
    m_menuBar->addMenu(m_toolsMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_toolsMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Spelling");
    add("Grammar");
    add("Thesaurus");
    add("Hyphenation");
    add("Language");
    add("Word Count");
    add("AutoCorrect");
    add("Mail Merge");
    add("Envelopes and Labels");
    add("Protect Document");
    add("Revisions");
    add("Macro");
    add("Customize");
    add("Options");
}

void Word7MenuBar::createTableMenu()
{
    m_tableMenu = new QMenu("&Table", m_parent);
    m_menuBar->addMenu(m_tableMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_tableMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Insert Table");
    add("Select Cell");
    add("Select Row");
    add("Select Column");
    add("Select Table");
    add("Split Cells");
    add("Split Table");
    add("Table Properties");
    add("Convert Text to Table");
}

void Word7MenuBar::createWindowMenu()
{
    m_windowMenu = new QMenu("&Window", m_parent);
    m_menuBar->addMenu(m_windowMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_windowMenu, text, text + " command");
        wireAction(action, text);
    };

    add("New Window");
    add("Arrange All");
    add("Split");
    add("Open Document Files");
}

void Word7MenuBar::createHelpMenu()
{
    m_helpMenu = new QMenu("&Help", m_parent);
    m_menuBar->addMenu(m_helpMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_helpMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Help Topics");
    add("Microsoft on the Web");
    add("About Microsoft Word");
}

void Word7MenuBar::createToolbars()
{
    auto* standard = new QToolBar("Standard", m_parent);
    standard->setObjectName("word95_standard_toolbar");
    standard->setMovable(true);
    standard->setFloatable(true);
    standard->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    auto addStandard = [standard](const QString& text) {
        addToolbarAction(standard, text, text + " command");
    };

    addStandard("New");
    addStandard("Open");
    addStandard("Save");
    addStandard("Print");
    addStandard("Cut");
    addStandard("Copy");
    addStandard("Paste");
    addStandard("Undo");
    addStandard("Redo");
    addStandard("Spelling");
    addStandard("Format Painter");
    m_parent->addToolBar(standard);

    m_parent->addToolBarBreak();

    auto* formatting = new QToolBar("Formatting", m_parent);
    formatting->setObjectName("word95_formatting_toolbar");
    formatting->setMovable(true);
    formatting->setFloatable(true);
    formatting->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    auto addFormatting = [formatting](const QString& text) {
        auto* action = addToolbarAction(formatting, text, text + " command");
        QObject::connect(action, &QAction::triggered, [text]() {
            qDebug("Formatting action: %s", qPrintable(text));
        });
    };

    addFormatting("Font");
    addFormatting("Size");
    addFormatting("Bold");
    addFormatting("Italic");
    addFormatting("Underline");
    addFormatting("Left");
    addFormatting("Center");
    addFormatting("Right");
    addFormatting("Justify");
    addFormatting("Bullets");
    addFormatting("Indent");
    m_parent->addToolBar(formatting);

    auto* drawing = new QToolBar("Drawing", m_parent);
    drawing->setObjectName("word95_drawing_toolbar");
    drawing->setMovable(true);
    drawing->setFloatable(true);
    drawing->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    auto addDrawing = [drawing](const QString& text) {
        addToolbarAction(drawing, text, text + " command");
    };

    addDrawing("Line");
    addDrawing("Rectangle");
    addDrawing("Oval");
    addDrawing("Text Box");
    addDrawing("Fill Color");
    addDrawing("Line Color");
    m_parent->addToolBar(drawing);
}

void Word7MenuBar::createRuler()
{
    auto* rulerToolbar = new QToolBar("Ruler", m_parent);
    rulerToolbar->setObjectName("word95_ruler_toolbar");
    rulerToolbar->setMovable(true);
    rulerToolbar->setFloatable(true);
    rulerToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    auto* ruler = new QWidget(m_parent);
    auto* rulerLayout = new QHBoxLayout(ruler);
    rulerLayout->setContentsMargins(8, 4, 8, 4);
    rulerLayout->setSpacing(6);

    auto* rulerText = new QLabel("0     1     2     3     4     5     6     7     8     9     10    11    12", ruler);
    rulerText->setStyleSheet("QLabel { color: #333; font-size: 11px; } ");
    rulerLayout->addWidget(rulerText);
    rulerLayout->addStretch();

    ruler->setStyleSheet("QWidget { background: #f0f0f0; border: 1px solid #bdbdbd; } ");
    rulerToolbar->addWidget(ruler);
    m_parent->addToolBar(rulerToolbar);

    auto* statusBar = m_parent->statusBar();
    if (statusBar) {
        auto* page = new QLabel("Page 1", statusBar);
        auto* section = new QLabel("Sec 1", statusBar);
        auto* pos = new QLabel("Ln 1, Col 1", statusBar);
        auto* mode = new QLabel("OVR", statusBar);
        page->setStyleSheet("QLabel { padding: 0 6px; }");
        section->setStyleSheet("QLabel { padding: 0 6px; }");
        pos->setStyleSheet("QLabel { padding: 0 6px; }");
        mode->setStyleSheet("QLabel { padding: 0 6px; }");

        statusBar->addPermanentWidget(page);
        statusBar->addPermanentWidget(section);
        statusBar->addPermanentWidget(pos);
        statusBar->addPermanentWidget(mode);
    }
}

} // namespace blastmaster::docs
