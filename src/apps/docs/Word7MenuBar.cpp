#include "Word7MenuBar.h"
#include "blastmaster/RecentFiles.h"
#include "Document.h"
#include "blastmaster/HelpTopicsWindow.h"

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QPlainTextEdit>
#include <QSignalBlocker>

namespace blastmaster::docs {

namespace {
QString normalizedActionName(const QString& text)
{
    QString value = text.toLower();
    value.replace("&", "");
    value.replace(" ", "_");
    value.replace("-", "_");
    value.replace(".", "_");
    value.replace("'", "");
    value = value.trimmed();
    return value;
}

QString iconPathForAction(const QString& text)
{
    const QString key = normalizedActionName(text);

    if (key == "new") return QStringLiteral(":/docs/icons/file/new.svg");
    if (key == "open") return QStringLiteral(":/docs/icons/file/open.svg");
    if (key == "save") return QStringLiteral(":/docs/icons/file/save.svg");
    if (key == "save_as") return QStringLiteral(":/docs/icons/file/save_as.svg");
    if (key == "close") return QStringLiteral(":/docs/icons/file/close.svg");
    if (key == "print") return QStringLiteral(":/docs/icons/file/print.svg");
    if (key == "export") return QStringLiteral(":/docs/icons/file/export.svg");

    if (key == "undo") return QStringLiteral(":/docs/icons/edit/undo.svg");
    if (key == "redo") return QStringLiteral(":/docs/icons/edit/redo.svg");
    if (key == "cut") return QStringLiteral(":/docs/icons/edit/cut.svg");
    if (key == "copy") return QStringLiteral(":/docs/icons/edit/copy.svg");
    if (key == "paste") return QStringLiteral(":/docs/icons/edit/paste.svg");
    if (key == "select_all") return QStringLiteral(":/docs/icons/edit/select_all.svg");
    if (key == "find") return QStringLiteral(":/docs/icons/edit/find.svg");
    if (key == "replace") return QStringLiteral(":/docs/icons/edit/replace.svg");

    if (key == "bold") return QStringLiteral(":/docs/icons/format/bold.svg");
    if (key == "italic") return QStringLiteral(":/docs/icons/format/italic.svg");
    if (key == "underline") return QStringLiteral(":/docs/icons/format/underline.svg");
    if (key == "strikethrough") return QStringLiteral(":/docs/icons/format/strikethrough.svg");
    if (key == "font") return QStringLiteral(":/docs/icons/format/text_color.svg");
    if (key == "paragraph") return QStringLiteral(":/docs/icons/paragraph/align_left.svg");
    if (key == "bullets") return QStringLiteral(":/docs/icons/paragraph/bullet_list.svg");
    if (key == "indent") return QStringLiteral(":/docs/icons/paragraph/indent_increase.svg");

    if (key == "picture") return QStringLiteral(":/docs/icons/insert/image.svg");
    if (key == "table") return QStringLiteral(":/docs/icons/insert/table.svg");
    if (key == "comment") return QStringLiteral(":/docs/icons/insert/comment.svg");
    if (key == "text_box") return QStringLiteral(":/docs/icons/insert/text_box.svg");
    if (key == "hyperlink") return QStringLiteral(":/docs/icons/insert/hyperlink.svg");

    if (key == "zoom") return QStringLiteral(":/docs/icons/view/zoom_in.svg");
    if (key == "ruler") return QStringLiteral(":/docs/icons/view/show_grid.svg");
    if (key == "toolbars") return QStringLiteral(":/docs/icons/tools/settings.svg");

    if (key == "spelling") return QStringLiteral(":/docs/icons/tools/spelling.svg");
    if (key == "grammar") return QStringLiteral(":/docs/icons/tools/grammar.svg");
    if (key == "word_count") return QStringLiteral(":/docs/icons/tools/word_count.svg");
    if (key == "preferences") return QStringLiteral(":/docs/icons/tools/settings.svg");
    if (key == "options") return QStringLiteral(":/docs/icons/tools/settings.svg");

    if (key == "help_topics") return QStringLiteral(":/docs/icons/app/help.svg");
    if (key == "about_microsoft_word") return QStringLiteral(":/docs/icons/app/about.svg");
    if (key == "about") return QStringLiteral(":/docs/icons/app/about.svg");

    return QString();
}

QAction* addMenuAction(QMenu* menu, const QString& text, const QString& statusTip = QString())
{
    auto* action = new QAction(text, menu);
    const QString iconPath = iconPathForAction(text);
    if (!iconPath.isEmpty()) {
        action->setIcon(QIcon(iconPath));
    }
    if (!statusTip.isEmpty()) {
        action->setStatusTip(statusTip);
    }
    menu->addAction(action);
    return action;
}

QAction* addToolbarAction(QToolBar* toolbar, const QString& text, const QString& statusTip = QString())
{
    auto* action = new QAction(text, toolbar);
    const QString iconPath = iconPathForAction(text);
    if (!iconPath.isEmpty()) {
        action->setIcon(QIcon(iconPath));
    }
    if (!statusTip.isEmpty()) {
        action->setStatusTip(statusTip);
    }
    toolbar->addAction(action);
    return action;
}
} // namespace

Word7MenuBar::Word7MenuBar(QMainWindow* parent, Document* document)
    : m_parent(parent)
    , m_document(document)
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

    auto* newAction = addMenuAction(m_fileMenu, "&New", "Create a new document");
    QObject::connect(newAction, &QAction::triggered, this, &Word7MenuBar::onFileNew);

    auto* openAction = addMenuAction(m_fileMenu, "&Open", "Open an existing document");
    QObject::connect(openAction, &QAction::triggered, this, &Word7MenuBar::onFileOpen);

    auto* recentMenu = m_fileMenu->addMenu("Recent Documents");
    QObject::connect(recentMenu, &QMenu::aboutToShow, [this, recentMenu]() {
        recentMenu->clear();
        const auto recent = blastmaster::RecentFiles::files(QStringLiteral("docs"));
        if (recent.isEmpty()) {
            auto* empty = recentMenu->addAction("(No recent documents)");
            empty->setEnabled(false);
            return;
        }
        for (const QString& path : recent) {
            auto* action = recentMenu->addAction(path);
            QObject::connect(action, &QAction::triggered, [this, path]() {
                m_document->setFilePath(path);
                if (!m_document->load()) {
                    QMessageBox::critical(m_parent, "Error", "Failed to open document: " + path);
                    return;
                }
                blastmaster::RecentFiles::add(QStringLiteral("docs"), path);
                m_parent->setWindowTitle(QString("Blastmaster Docs - %1").arg(m_document->title()));
                m_parent->statusBar()->showMessage("Document opened: " + path);
            });
        }
    });

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_fileMenu, text, text + " command");
        wireAction(action, text);
    };

    add("&Close");

    auto* saveAction = addMenuAction(m_fileMenu, "&Save", "Save the current document");
    QObject::connect(saveAction, &QAction::triggered, this, &Word7MenuBar::onFileSave);

    auto* saveAsAction = addMenuAction(m_fileMenu, "Save &As", "Save the document with a new name");
    QObject::connect(saveAsAction, &QAction::triggered, this, &Word7MenuBar::onFileSaveAs);

    add("&Version");
    add("Find File");
    add("Page Setup");
    add("Print Preview");
    add("Print");
    add("Send");
    add("Properties");

    m_fileMenu->addSeparator();
    auto* exitAction = addMenuAction(m_fileMenu, "E&xit", "Exit the application");
    QObject::connect(exitAction, &QAction::triggered, [this]() {
        m_parent->close();
    });
}

void Word7MenuBar::onFileNew()
{
    if (m_document->isDirty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(m_parent,
            "Save Changes?",
            "The document has been modified. Do you want to save changes?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            return;
        }
        if (reply == QMessageBox::Yes) {
            onFileSave();
        }
    }

    m_document->setTitle("Untitled Document");
    m_document->setContent("");
    m_document->setFilePath("");
    m_document->setClean();
    if (auto* editor = m_parent->findChild<QPlainTextEdit*>("documentEditor")) { QSignalBlocker blocker(editor); editor->setPlainText(QString()); }
    m_parent->setWindowTitle("Blastmaster Docs - Untitled");
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("New document created");
    }
}

void Word7MenuBar::onFileOpen()
{
    QFileDialog dialog(m_parent, "Open Document", QString(), Document::fileFilter());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDefaultSuffix(Document::defaultSuffix());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString filePath = dialog.selectedFiles().first();
    m_document->setFilePath(filePath);
    if (!m_document->load()) {
        QMessageBox::critical(m_parent, "Error", "Failed to open document: " + filePath);
        return;
    }

    if (auto* editor = m_parent->findChild<QPlainTextEdit*>("documentEditor")) { QSignalBlocker blocker(editor); editor->setPlainText(m_document->content()); }
    m_parent->setWindowTitle(QString("Blastmaster Docs - %1").arg(m_document->title()));
    blastmaster::RecentFiles::add(QStringLiteral("docs"), filePath);
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("Document opened: " + filePath);
    }
}

void Word7MenuBar::onFileSave()
{
    if (m_document->filePath().isEmpty()) {
        onFileSaveAs();
        return;
    }

    if (!m_document->save()) {
        QMessageBox::critical(m_parent, "Error", "Failed to save document");
        return;
    }

    blastmaster::RecentFiles::add(QStringLiteral("docs"), m_document->filePath());
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("Document saved: " + m_document->filePath());
    }
}

void Word7MenuBar::onFileSaveAs()
{
    QFileDialog dialog(m_parent, "Save Document As", QString(), Document::fileFilter());
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(Document::defaultSuffix());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString filePath = dialog.selectedFiles().first();
    if (!m_document->saveAs(filePath)) {
        QMessageBox::critical(m_parent, "Error", "Failed to save document to: " + filePath);
        return;
    }

    m_parent->setWindowTitle(QString("Blastmaster Docs - %1").arg(m_document->title()));
    blastmaster::RecentFiles::add(QStringLiteral("docs"), filePath);
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("Document saved as: " + filePath);
    }
}

void Word7MenuBar::onFileExit()
{
    m_parent->close();
}

void Word7MenuBar::createEditMenu()
{
    m_editMenu = new QMenu("&Edit", m_parent);
    m_menuBar->addMenu(m_editMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_editMenu, text, text + " command");
        wireAction(action, text);
        return action;
    };

    auto* undo = add("&Undo");
    QObject::connect(undo, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->undo(); });
    auto* redo = add("&Repeat");
    QObject::connect(redo, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->redo(); });
    auto* cut = add("Cu&t");
    QObject::connect(cut, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->cut(); });
    auto* copy = add("&Copy");
    QObject::connect(copy, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->copy(); });
    auto* paste = add("&Paste");
    QObject::connect(paste, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->paste(); });
    add("Paste Special");
    auto* clear = add("Clear");
    QObject::connect(clear, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->clear(); });
    auto* selectAll = add("Select All");
    QObject::connect(selectAll, &QAction::triggered, [this] { if (auto* e = m_parent->findChild<QPlainTextEdit*>("documentEditor")) e->selectAll(); });
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

    auto* helpTopics = addMenuAction(m_helpMenu, "Help Topics", "Open Blastmaster Docs Help Topics");
    QObject::connect(helpTopics, &QAction::triggered, [this]() {
        blastmaster::HelpTopicsWindow::showFor(m_parent, "docs");
    });
    add("Microsoft on the Web");
    auto* about = addMenuAction(m_helpMenu, "About Microsoft Word", "About Blastmaster Docs");
    QObject::connect(about, &QAction::triggered, [this]() {
        QMessageBox::about(m_parent, "About Blastmaster Docs", "Blastmaster Docs\nVersion 1.0.0\nClassic office-style word processor.");
    });
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

