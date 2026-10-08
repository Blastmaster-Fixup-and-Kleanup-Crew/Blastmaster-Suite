#include "Access95MenuBar.h"
#include "DatabaseDocument.h"
#include "blastmaster/HelpTopicsWindow.h"
#include "blastmaster/RecentFiles.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QFontComboBox>
#include <QInputDialog>
#include <QKeySequence>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QFileInfo>

namespace blastmaster::database {

static QAction* action(QObject* parent, const QString& text,
                       const QKeySequence& shortcut = QKeySequence())
{
    auto* a = new QAction(text, parent);
    if (!shortcut.isEmpty()) a->setShortcut(shortcut);
    return a;
}

Access95MenuBar::Access95MenuBar(QMainWindow* parent, DatabaseDocument* document)
    : m_parent(parent), m_document(document)
{
    m_menuBar = new QMenuBar(parent);
    createFileMenu();
    createEditMenu();
    createViewMenu();
    createInsertMenu();
    createFormatMenu();
    createToolsMenu();
    createWindowMenu();
    createHelpMenu();
    createToolbars();
}

void Access95MenuBar::createFileMenu()
{
    m_fileMenu = m_menuBar->addMenu(QStringLiteral("&File"));

    auto* aNew = action(m_parent, QStringLiteral("&New"));
    auto* aOpen = action(m_parent, QStringLiteral("&Open..."), QKeySequence::Open);
    auto* aClose = action(m_parent, QStringLiteral("C&lose"), QKeySequence(QStringLiteral("Ctrl+W")));
    auto* aSave = action(m_parent, QStringLiteral("&Save"), QKeySequence::Save);
    auto* aSaveAs = action(m_parent, QStringLiteral("Save &As..."), QKeySequence::SaveAs);
    auto* aHtml = action(m_parent, QStringLiteral("Save As &HTML..."));
    auto* aPage = action(m_parent, QStringLiteral("Page Set&up..."));
    auto* aPreview = action(m_parent, QStringLiteral("Print Pre&view"));
    auto* aPrint = action(m_parent, QStringLiteral("&Print..."), QKeySequence::Print);
    auto* aSend = action(m_parent, QStringLiteral("&Send"));
    auto* aProperties = action(m_parent, QStringLiteral("Database &Properties..."));
    auto* aExit = action(m_parent, QStringLiteral("E&xit"), QKeySequence::Quit);

    m_fileMenu->addAction(aNew);
    m_fileMenu->addAction(aOpen);
    m_fileMenu->addAction(aClose);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aSave);
    m_fileMenu->addAction(aSaveAs);
    m_fileMenu->addAction(aHtml);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aPage);
    m_fileMenu->addAction(aPreview);
    m_fileMenu->addAction(aPrint);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aSend);
    m_fileMenu->addAction(aProperties);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(aExit);

    QObject::connect(aNew, &QAction::triggered, [this] {
        if (m_document->newDatabase())
            m_parent->statusBar()->showMessage(QStringLiteral("New database created."), 2500);
    });
    QObject::connect(aOpen, &QAction::triggered, [this] {
        const QString path = QFileDialog::getOpenFileName(
            m_parent, QStringLiteral("Open Database"), QString(),
            QStringLiteral("SQLite databases (*.db *.sqlite *.sqlite3);;All files (*.*)"));
        if (!path.isEmpty() && m_document->openDatabase(path)) {
            blastmaster::RecentFiles::add(QStringLiteral("databases"), path);
            m_parent->statusBar()->showMessage(QStringLiteral("Database opened."), 2500);
        }
    });
    QObject::connect(aSave, &QAction::triggered, [this] {
        if (m_document->filePath().isEmpty()) {
            aSaveAs->trigger();
        } else if (m_document->saveDatabase()) {
            blastmaster::RecentFiles::add(QStringLiteral("databases"), m_document->filePath());
            m_parent->statusBar()->showMessage(QStringLiteral("Database saved."), 2500);
        }
    });
    QObject::connect(aSaveAs, &QAction::triggered, [this] {
        const QString path = QFileDialog::getSaveFileName(
            m_parent, QStringLiteral("Save Database As"), QStringLiteral("Database.db"),
            QStringLiteral("SQLite databases (*.db *.sqlite *.sqlite3);;All files (*.*)"));
        if (!path.isEmpty() && m_document->saveDatabaseAs(path)) {
            blastmaster::RecentFiles::add(QStringLiteral("databases"), path);
            m_parent->statusBar()->showMessage(QStringLiteral("Database saved."), 2500);
        }
    });
    QObject::connect(aClose, &QAction::triggered, [this] { m_document->closeDatabase(); });
    QObject::connect(aExit, &QAction::triggered, qApp, &QApplication::quit);
    QObject::connect(aProperties, &QAction::triggered, [this] {
        QMessageBox::information(m_parent, QStringLiteral("Database Properties"),
                                 QStringLiteral("Blastmaster Database\nSQLite database engine"));
    });
    QObject::connect(aHtml, &QAction::triggered, [this] {
        m_parent->statusBar()->showMessage(QStringLiteral("HTML export is not implemented yet."), 3000);
    });
    QObject::connect(aPage, &QAction::triggered, [this] {
        m_parent->statusBar()->showMessage(QStringLiteral("Page Setup is not implemented yet."), 3000);
    });
    QObject::connect(aPreview, &QAction::triggered, [this] {
        m_parent->statusBar()->showMessage(QStringLiteral("Print Preview is not implemented yet."), 3000);
    });
    QObject::connect(aPrint, &QAction::triggered, [this] {
        m_parent->statusBar()->showMessage(QStringLiteral("Printing is not implemented yet."), 3000);
    });
    QObject::connect(aSend, &QAction::triggered, [this] {
        m_parent->statusBar()->showMessage(QStringLiteral("Send is not implemented yet."), 3000);
    });
}

void Access95MenuBar::createEditMenu()
{
    m_editMenu = m_menuBar->addMenu(QStringLiteral("&Edit"));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Undo"), QKeySequence::Undo));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Repeat"), QKeySequence::Redo));
    m_editMenu->addSeparator();
    m_editMenu->addAction(action(m_parent, QStringLiteral("Cu&t"), QKeySequence::Cut));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Copy"), QKeySequence::Copy));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Paste"), QKeySequence::Paste));
    m_editMenu->addAction(action(m_parent, QStringLiteral("Paste &Special...")));
    m_editMenu->addAction(action(m_parent, QStringLiteral("C&lear")));
    m_editMenu->addAction(action(m_parent, QStringLiteral("Select &All"), QKeySequence::SelectAll));
    m_editMenu->addSeparator();
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Find..."), QKeySequence::Find));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Replace..."), QKeySequence::Replace));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Delete")));
    m_editMenu->addAction(action(m_parent, QStringLiteral("&Object...")));
}

void Access95MenuBar::createViewMenu()
{
    m_viewMenu = m_menuBar->addMenu(QStringLiteral("&View"));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Design View")));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Datasheet View")));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Form View")));
    m_viewMenu->addSeparator();

    auto* objects = m_viewMenu->addMenu(QStringLiteral("Database &Objects"));
    objects->addAction(action(m_parent, QStringLiteral("&Table")));
    objects->addAction(action(m_parent, QStringLiteral("&Query")));
    objects->addAction(action(m_parent, QStringLiteral("&Form")));
    objects->addAction(action(m_parent, QStringLiteral("&Report")));
    objects->addAction(action(m_parent, QStringLiteral("&Macro")));
    objects->addAction(action(m_parent, QStringLiteral("&Module")));

    m_viewMenu->addSeparator();
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Sorting...")));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Filtering...")));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Toolbars...")));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Ruler")));
    m_viewMenu->addAction(action(m_parent, QStringLiteral("&Grid")));
}

void Access95MenuBar::createInsertMenu()
{
    m_insertMenu = m_menuBar->addMenu(QStringLiteral("&Insert"));
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Table")));
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Query")));
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Form")));
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Report")));
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Macro")));
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Module")));
    m_insertMenu->addSeparator();
    m_insertMenu->addAction(action(m_parent, QStringLiteral("&Object...")));
}

void Access95MenuBar::createFormatMenu()
{
    m_formatMenu = m_menuBar->addMenu(QStringLiteral("F&ormat"));
    m_formatMenu->addAction(action(m_parent, QStringLiteral("&Font...")));
    m_formatMenu->addAction(action(m_parent, QStringLiteral("&Datasheet")));
    m_formatMenu->addAction(action(m_parent, QStringLiteral("Size to &Fit")));
    m_formatMenu->addAction(action(m_parent, QStringLiteral("&Colors")));
}

void Access95MenuBar::createToolsMenu()
{
    m_toolsMenu = m_menuBar->addMenu(QStringLiteral("&Tools"));

    m_toolsMenu->addAction(action(m_parent, QStringLiteral("&Spelling...")));
    m_toolsMenu->addAction(action(m_parent, QStringLiteral("&AutoCorrect...")));
    m_toolsMenu->addSeparator();

    auto* security = m_toolsMenu->addMenu(QStringLiteral("&Security"));
    security->addAction(action(m_parent, QStringLiteral("&User and Group Permissions...")));
    security->addAction(action(m_parent, QStringLiteral("&User and Group Accounts...")));

    m_toolsMenu->addSeparator();
    auto* utilities = m_toolsMenu->addMenu(QStringLiteral("Database &Utilities"));
    utilities->addAction(action(m_parent, QStringLiteral("&Compact and Repair Database...")));
    utilities->addAction(action(m_parent, QStringLiteral("&Convert Database...")));

    m_toolsMenu->addAction(action(m_parent, QStringLiteral("&Replicate")));
    m_toolsMenu->addAction(action(m_parent, QStringLiteral("&Macros")));
    m_toolsMenu->addAction(action(m_parent, QStringLiteral("&Add-Ins")));
    m_toolsMenu->addAction(action(m_parent, QStringLiteral("&Options...")));
}

void Access95MenuBar::createWindowMenu()
{
    m_windowMenu = m_menuBar->addMenu(QStringLiteral("&Window"));
    m_windowMenu->addAction(action(m_parent, QStringLiteral("&New Window")));
    m_windowMenu->addAction(action(m_parent, QStringLiteral("&Cascade")));
    m_windowMenu->addAction(action(m_parent, QStringLiteral("Tile &Horizontal")));
    m_windowMenu->addAction(action(m_parent, QStringLiteral("Tile &Vertical")));
    m_windowMenu->addAction(action(m_parent, QStringLiteral("&Arrange Icons")));
    m_windowMenu->addSeparator();

    auto* openObjects = m_windowMenu->addMenu(QStringLiteral("Open &Objects"));
    openObjects->addAction(action(m_parent, QStringLiteral("(No open objects)")));
}

void Access95MenuBar::createHelpMenu()
{
    m_helpMenu = m_menuBar->addMenu(QStringLiteral("&Help"));
    auto* helpTopics = action(m_parent, QStringLiteral("Microsoft Access Help &Topics"), QKeySequence::HelpContents);
    m_helpMenu->addAction(helpTopics);
    QObject::connect(helpTopics, &QAction::triggered, [this] {
        blastmaster::HelpTopicsWindow::showFor(m_parent, QStringLiteral("databases"));
    });
    m_helpMenu->addAction(action(m_parent, QStringLiteral("&About Microsoft Access")));
    QObject::connect(m_helpMenu->actions().last(), &QAction::triggered, [this] {
        QMessageBox::about(m_parent, QStringLiteral("About Blastmaster Database"),
                           QStringLiteral("Blastmaster Database\nClassic Access 95-style database application."));
    });
}

void Access95MenuBar::createToolbars()
{
    auto* standard = new QToolBar(QStringLiteral("Database"), m_parent);
    standard->setObjectName(QStringLiteral("toolbar_database"));
    standard->setMovable(true);
    standard->setFloatable(true);
    standard->setIconSize(QSize(16, 16));

    auto addToolbarAction = [this](QToolBar* toolbar, const QString& text,
                                  const QKeySequence& seq = QKeySequence()) {
        auto* a = action(toolbar, text, seq);
        toolbar->addAction(a);
        QObject::connect(a, &QAction::triggered, [this, text] {
            m_parent->statusBar()->showMessage(text, 2000);
        });
        return a;
    };

    addToolbarAction(standard, QStringLiteral("New Database"));
    addStatusAction(QStringLiteral("Open"), QKeySequence::Open);
    addStatusAction(QStringLiteral("Save"), QKeySequence::Save);
    addStatusAction(QStringLiteral("Print"));
    addStatusAction(QStringLiteral("Print Preview"));
    standard->addSeparator();
    addStatusAction(QStringLiteral("Cut"), QKeySequence::Cut);
    addStatusAction(QStringLiteral("Copy"), QKeySequence::Copy);
    addStatusAction(QStringLiteral("Paste"), QKeySequence::Paste);
    addStatusAction(QStringLiteral("Format Painter"));
    standard->addSeparator();
    addStatusAction(QStringLiteral("Undo"), QKeySequence::Undo);
    addStatusAction(QStringLiteral("Spelling"));
    addStatusAction(QStringLiteral("Relationships"));
    addStatusAction(QStringLiteral("Database Window"));
    addStatusAction(QStringLiteral("Table Wizard"));
    addStatusAction(QStringLiteral("Query Wizard"));
    addStatusAction(QStringLiteral("Form Wizard"));
    addStatusAction(QStringLiteral("Report Wizard"));
    m_parent->addToolBar(Qt::TopToolBarArea, standard);

    auto* formatting = new QToolBar(QStringLiteral("Formatting"), m_parent);
    formatting->setObjectName(QStringLiteral("toolbar_formatting"));
    formatting->setMovable(true);
    formatting->setFloatable(true);

    auto* font = new QFontComboBox(formatting);
    font->setCurrentFont(QFont(QStringLiteral("MS Sans Serif"), 8));
    formatting->addWidget(font);

    auto* size = new QComboBox(formatting);
    for (int pointSize : {8, 9, 10, 11, 12, 14, 16, 18, 20, 24, 28, 36})
        size->addItem(QString::number(pointSize));
    size->setCurrentText(QStringLiteral("8"));
    formatting->addWidget(size);

    for (const QString& text : {QStringLiteral("Bold"), QStringLiteral("Italic"),
                                QStringLiteral("Underline"), QStringLiteral("Align Left"),
                                QStringLiteral("Center"), QStringLiteral("Align Right"),
                                QStringLiteral("Font Color"), QStringLiteral("Special Effect")}) {
        addToolbarAction(formatting, text);
    }

    m_parent->addToolBar(Qt::TopToolBarArea, formatting);
}

} // namespace blastmaster::database
