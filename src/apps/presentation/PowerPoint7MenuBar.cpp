#include "PowerPoint7MenuBar.h"
#include "PresentationDocument.h"
#include "blastmaster/HelpTopicsWindow.h"
#include "blastmaster/RecentFiles.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QFileInfo>

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

    if (key == "new") return QStringLiteral(":/presentation/icons/file/new.svg");
    if (key == "open") return QStringLiteral(":/presentation/icons/file/open.svg");
    if (key == "save") return QStringLiteral(":/presentation/icons/file/save.svg");
    if (key == "print") return QStringLiteral(":/presentation/icons/file/print.svg");
    if (key == "close") return QStringLiteral(":/presentation/icons/file/close.svg");

    if (key == "undo") return QStringLiteral(":/presentation/icons/edit/undo.svg");
    if (key == "redo") return QStringLiteral(":/presentation/icons/edit/redo.svg");
    if (key == "cut") return QStringLiteral(":/presentation/icons/edit/cut.svg");
    if (key == "copy") return QStringLiteral(":/presentation/icons/edit/copy.svg");
    if (key == "paste") return QStringLiteral(":/presentation/icons/edit/paste.svg");
    if (key == "find") return QStringLiteral(":/presentation/icons/edit/find.svg");
    if (key == "replace") return QStringLiteral(":/presentation/icons/edit/replace.svg");

    if (key == "bold") return QStringLiteral(":/presentation/icons/format/bold.svg");
    if (key == "italic") return QStringLiteral(":/presentation/icons/format/italic.svg");
    if (key == "underline") return QStringLiteral(":/presentation/icons/format/underline.svg");
    if (key == "font") return QStringLiteral(":/presentation/icons/format/font.svg");
    if (key == "bullets") return QStringLiteral(":/presentation/icons/format/bullets.svg");

    if (key == "line") return QStringLiteral(":/presentation/icons/draw/line.svg");
    if (key == "rectangle") return QStringLiteral(":/presentation/icons/draw/rectangle.svg");
    if (key == "oval") return QStringLiteral(":/presentation/icons/draw/oval.svg");
    if (key == "text_box") return QStringLiteral(":/presentation/icons/draw/text_box.svg");
    if (key == "fill_color") return QStringLiteral(":/presentation/icons/draw/fill.svg");
    if (key == "line_color") return QStringLiteral(":/presentation/icons/draw/line_color.svg");

    if (key == "zoom") return QStringLiteral(":/presentation/icons/view/zoom.svg");
    if (key == "ruler") return QStringLiteral(":/presentation/icons/view/ruler.svg");
    if (key == "toolbars") return QStringLiteral(":/presentation/icons/view/toolbars.svg");

    if (key == "spelling") return QStringLiteral(":/presentation/icons/tools/spelling.svg");
    if (key == "options") return QStringLiteral(":/presentation/icons/tools/options.svg");

    if (key == "help_topics") return QStringLiteral(":/presentation/icons/app/help.svg");
    if (key == "about") return QStringLiteral(":/presentation/icons/app/about.svg");

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

namespace blastmaster::presentation {

PowerPoint7MenuBar::PowerPoint7MenuBar(QMainWindow* parent, PresentationDocument* document)
    : m_parent(parent)
    , m_document(document)
    , m_menuBar(new QMenuBar(parent))
    , m_fileMenu(nullptr)
    , m_editMenu(nullptr)
    , m_viewMenu(nullptr)
    , m_insertMenu(nullptr)
    , m_formatMenu(nullptr)
    , m_toolsMenu(nullptr)
    , m_slideShowMenu(nullptr)
    , m_windowMenu(nullptr)
    , m_helpMenu(nullptr)
{
    parent->setMenuBar(m_menuBar);
    if (!parent->statusBar()) {
        parent->setStatusBar(new QStatusBar(parent));
    }

    parent->setStyleSheet(R"(
        QMenuBar {
            background: #d4d0c8;
            color: #000000;
            border: 1px solid #ffffff;
            border-bottom: 1px solid #808080;
        }
        QMenuBar::item {
            background: transparent;
            padding: 4px 10px;
            margin: 0px;
        }
        QMenuBar::item:selected {
            background: #000080;
            color: #ffffff;
        }
        QMenu {
            background: #d4d0c8;
            color: #000000;
            border: 1px solid #808080;
            padding: 2px;
        }
        QMenu::item {
            padding: 3px 20px 3px 20px;
        }
        QMenu::item:selected {
            background: #000080;
            color: #ffffff;
        }
        QToolBar {
            background: #d4d0c8;
            border: 1px solid #808080;
            spacing: 3px;
        }
        QToolButton {
            background: #d4d0c8;
            color: #000000;
            border: 1px solid #ffffff;
            padding: 3px 5px;
        }
        QToolButton:hover {
            background: #ece9e4;
        }
        QToolButton:pressed {
            background: #c0c0c0;
            border: 1px solid #808080;
        }
        QLabel {
            color: #222222;
        }
    )");

    createFileMenu();
    createEditMenu();
    createViewMenu();
    createInsertMenu();
    createFormatMenu();
    createToolsMenu();
    createSlideShowMenu();
    createWindowMenu();
    createHelpMenu();
    createToolbars();
    createStatusWidgets();

    parent->statusBar()->showMessage("Presentation initialized");
}

void PowerPoint7MenuBar::wireAction(QAction* action, const QString& label)
{
    QObject::connect(action, &QAction::triggered, [this, label]() {
        if (m_parent && m_parent->statusBar()) {
            m_parent->statusBar()->showMessage(label + " selected");
        }
    });
}

void PowerPoint7MenuBar::createFileMenu()
{
    m_fileMenu = new QMenu("&File", m_parent);
    m_menuBar->addMenu(m_fileMenu);

    auto* newAction = addMenuAction(m_fileMenu, "&New", "Create a new presentation");
    QObject::connect(newAction, &QAction::triggered, [this]() { onFileNew(); });

    auto* openAction = addMenuAction(m_fileMenu, "&Open", "Open an existing presentation");
    QObject::connect(openAction, &QAction::triggered, [this]() { onFileOpen(); });

    auto* recentMenu = m_fileMenu->addMenu("Recent Presentations");
    QObject::connect(recentMenu, &QMenu::aboutToShow, [this, recentMenu]() {
        recentMenu->clear();
        const auto recent = blastmaster::RecentFiles::files(QStringLiteral("presentations"));
        if (recent.isEmpty()) {
            auto* empty = recentMenu->addAction("(No recent presentations)");
            empty->setEnabled(false);
            return;
        }
        for (const QString& path : recent) {
            auto* item = recentMenu->addAction(QFileInfo(path).fileName());
            item->setToolTip(path);
            QObject::connect(item, &QAction::triggered, [this, path]() {
                m_document->setFilePath(path);
                if (!m_document->load()) {
                    QMessageBox::critical(m_parent, "Error", "Failed to open presentation: " + path);
                    return;
                }
                blastmaster::RecentFiles::add(QStringLiteral("presentations"), path);
                m_parent->setWindowTitle(QString("Blastmaster Presentations - %1").arg(m_document->title()));
            });
        }
        recentMenu->addSeparator();
        auto* clear = recentMenu->addAction("Clear Recent Presentations");
        QObject::connect(clear, &QAction::triggered, [recentMenu]() {
            blastmaster::RecentFiles::clear(QStringLiteral("presentations"));
            recentMenu->hide();
        });
    });

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_fileMenu, text, text + " command");
        wireAction(action, text);
    };

    add("&Close");

    auto* saveAction = addMenuAction(m_fileMenu, "&Save", "Save the current presentation");
    QObject::connect(saveAction, &QAction::triggered, [this]() { onFileSave(); });

    auto* saveAsAction = addMenuAction(m_fileMenu, "Save &As", "Save the presentation with a new name");
    QObject::connect(saveAsAction, &QAction::triggered, [this]() { onFileSaveAs(); });

    add("Pack and Go");
    add("Page Setup");
    add("Print Setup");
    add("Print");
    add("Send");
    add("Properties");

    m_fileMenu->addSeparator();
    auto* exitAction = addMenuAction(m_fileMenu, "E&xit", "Exit the application");
    QObject::connect(exitAction, &QAction::triggered, [this]() { onFileExit(); });
}

void PowerPoint7MenuBar::onFileNew()
{
    if (m_document && m_document->isDirty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(m_parent,
            "Save Changes?",
            "The presentation has been modified. Do you want to save changes?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            return;
        }
        if (reply == QMessageBox::Yes) {
            onFileSave();
        }
    }

    m_document->setTitle("Untitled Presentation");
    m_document->setContent("");
    m_document->setFilePath("");
    m_document->setClean();
    m_parent->setWindowTitle("Blastmaster Presentations - Untitled");
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("New presentation created");
    }
}

void PowerPoint7MenuBar::onFileOpen()
{
    QFileDialog dialog(m_parent, "Open Presentation", QString(), PresentationDocument::fileFilter());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDefaultSuffix(PresentationDocument::defaultSuffix());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString filePath = dialog.selectedFiles().first();
    m_document->setFilePath(filePath);
    if (!m_document->load()) {
        QMessageBox::critical(m_parent, "Error", "Failed to open presentation: " + filePath);
        return;
    }

    m_parent->setWindowTitle(QString("Blastmaster Presentations - %1").arg(m_document->title()));
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("Presentation opened: " + filePath);
    }
    blastmaster::RecentFiles::add(QStringLiteral("presentations"), filePath);
}

void PowerPoint7MenuBar::onFileSave()
{
    if (!m_document) {
        return;
    }
    if (m_document->filePath().isEmpty()) {
        onFileSaveAs();
        return;
    }

    if (!m_document->save()) {
        QMessageBox::critical(m_parent, "Error", "Failed to save presentation");
        return;
    }

    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("Presentation saved: " + m_document->filePath());
    }
    blastmaster::RecentFiles::add(QStringLiteral("presentations"), m_document->filePath());
}

void PowerPoint7MenuBar::onFileSaveAs()
{
    QFileDialog dialog(m_parent, "Save Presentation As", QString(), PresentationDocument::fileFilter());
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(PresentationDocument::defaultSuffix());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString filePath = dialog.selectedFiles().first();
    if (!m_document->saveAs(filePath)) {
        QMessageBox::critical(m_parent, "Error", "Failed to save presentation to: " + filePath);
        return;
    }

    m_parent->setWindowTitle(QString("Blastmaster Presentations - %1").arg(m_document->title()));
    if (m_parent->statusBar()) {
        m_parent->statusBar()->showMessage("Presentation saved as: " + filePath);
    }
    blastmaster::RecentFiles::add(QStringLiteral("presentations"), filePath);
}

void PowerPoint7MenuBar::onFileExit()
{
    m_parent->close();
}

void PowerPoint7MenuBar::createEditMenu()
{
    m_editMenu = new QMenu("&Edit", m_parent);
    m_menuBar->addMenu(m_editMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_editMenu, text, text + " command");
        wireAction(action, text);
    };

    add("&Undo");
    add("&Redo");
    add("Cu&t");
    add("&Copy");
    add("&Paste");
    add("Paste Special");
    add("Clear");
    add("Select All");
    add("Duplicate");
    auto* deleteSlide = addMenuAction(m_editMenu, "Delete Slide", "Delete the selected slide");
    QObject::connect(deleteSlide, &QAction::triggered, [this]() {
        m_document->removeSlide(m_document->currentSlide());
        m_parent->statusBar()->showMessage("Slide deleted", 2000);
    });
    add("Find");
    add("Replace");
    add("Object");
}

void PowerPoint7MenuBar::createViewMenu()
{
    m_viewMenu = new QMenu("&View", m_parent);
    m_menuBar->addMenu(m_viewMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_viewMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Normal");
    add("Slide View");
    add("Outline View");
    add("Slide Sorter");
    add("Notes Page");
    add("Slide Show");
    add("Black and White");
    add("Toolbars");
    add("Ruler");
    add("Guides");
    add("Zoom");
}

void PowerPoint7MenuBar::createInsertMenu()
{
    m_insertMenu = new QMenu("&Insert", m_parent);
    m_menuBar->addMenu(m_insertMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_insertMenu, text, text + " command");
        wireAction(action, text);
    };

    auto* newSlide = addMenuAction(m_insertMenu, "New Slide", "Insert a new slide");
    QObject::connect(newSlide, &QAction::triggered, [this]() {
        m_document->addSlide();
        m_parent->statusBar()->showMessage("New slide inserted", 2000);
    });
    auto* duplicate = addMenuAction(m_insertMenu, "Duplicate Slide", "Duplicate the current slide");
    QObject::connect(duplicate, &QAction::triggered, [this]() {
        if (m_document->slideCount() > 0) {
            const auto& source = m_document->slide(m_document->currentSlide());
            m_document->addSlide(source.title + " Copy", source.body);
            m_parent->statusBar()->showMessage("Slide duplicated", 2000);
        }
    });
    add("Slide Numbers");
    add("Date and Time");
    add("Symbol");
    add("Object");
    add("Movie and Sound");
    add("Microsoft Excel Worksheet");
}

void PowerPoint7MenuBar::createFormatMenu()
{
    m_formatMenu = new QMenu("&Format", m_parent);
    m_menuBar->addMenu(m_formatMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_formatMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Font");
    add("Bullet");
    add("Alignment");
    add("Line Spacing");
    add("Change Case");
    add("Text Direction");
    add("Slide Color Scheme");
    add("Slide Background");
    add("Apply Design Template");
}

void PowerPoint7MenuBar::createToolsMenu()
{
    m_toolsMenu = new QMenu("&Tools", m_parent);
    m_menuBar->addMenu(m_toolsMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_toolsMenu, text, text + " command");
        wireAction(action, text);
    };

    add("Spelling");
    add("Style Checker");
    add("AutoContent Wizard");
    add("Meeting Minder");
    add("Options");
    add("Add-Ins");
    add("Macro");
}

void PowerPoint7MenuBar::createSlideShowMenu()
{
    m_slideShowMenu = new QMenu("&Slide Show", m_parent);
    m_menuBar->addMenu(m_slideShowMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_slideShowMenu, text, text + " command");
        wireAction(action, text);
    };

    add("View Show");
    add("Rehearse Timings");
    add("Slide Transition");
    add("Hide Slide");
    add("Set Up Show");
}

void PowerPoint7MenuBar::createWindowMenu()
{
    m_windowMenu = new QMenu("&Window", m_parent);
    m_menuBar->addMenu(m_windowMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_windowMenu, text, text + " command");
        wireAction(action, text);
    };

    add("New Window");
    add("Cascade");
    add("Tile");
    add("Arrange Icons");
    add("Open Windows List");
}

void PowerPoint7MenuBar::createHelpMenu()
{
    m_helpMenu = new QMenu("&Help", m_parent);
    m_menuBar->addMenu(m_helpMenu);

    auto add = [this](const QString& text) {
        QAction* action = addMenuAction(m_helpMenu, text, text + " command");
        wireAction(action, text);
    };

    auto* helpTopics = addMenuAction(m_helpMenu, "Contents and Index", "Open Blastmaster Presentations Help Topics");
    QObject::connect(helpTopics, &QAction::triggered, [this]() {
        blastmaster::HelpTopicsWindow::showFor(m_parent, "presentations");
    });
    add("Getting Assistance");
    add("Microsoft on the Web");
    auto* about = addMenuAction(m_helpMenu, "About Microsoft PowerPoint", "About Blastmaster Presentations");
    QObject::connect(about, &QAction::triggered, [this]() {
        QMessageBox::about(m_parent, "About Blastmaster Presentations", "Blastmaster Presentations\nVersion 1.0.0\nClassic office-style presentation editor.");
    });
}

void PowerPoint7MenuBar::createToolbars()
{
    auto* standard = new QToolBar("Standard", m_parent);
    standard->setObjectName("ppt7_standard_toolbar");
    standard->setMovable(true);
    standard->setFloatable(true);
    standard->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    for (const auto& label : {QStringLiteral("New"), QStringLiteral("Open"), QStringLiteral("Save"), QStringLiteral("Print"),
                             QStringLiteral("Spelling"), QStringLiteral("Cut"), QStringLiteral("Copy"), QStringLiteral("Paste"),
                             QStringLiteral("Format Painter"), QStringLiteral("Undo"), QStringLiteral("Redo"),
                             QStringLiteral("Insert Chart"), QStringLiteral("Insert Clip Art"), QStringLiteral("Zoom")}) {
        addToolbarAction(standard, label, label + " command");
    }
    m_parent->addToolBar(standard);

    m_parent->addToolBarBreak();

    auto* formatting = new QToolBar("Formatting", m_parent);
    formatting->setObjectName("ppt7_formatting_toolbar");
    formatting->setMovable(true);
    formatting->setFloatable(true);
    formatting->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    for (const auto& label : {QStringLiteral("Font"), QStringLiteral("Size"), QStringLiteral("Bold"), QStringLiteral("Italic"),
                             QStringLiteral("Underline"), QStringLiteral("Left"), QStringLiteral("Center"), QStringLiteral("Right"),
                             QStringLiteral("Bullets"), QStringLiteral("Promote"), QStringLiteral("Demote"), QStringLiteral("Font Color")}) {
        addToolbarAction(formatting, label, label + " command");
    }
    m_parent->addToolBar(formatting);

    auto* drawing = new QToolBar("Drawing", m_parent);
    drawing->setObjectName("ppt7_drawing_toolbar");
    drawing->setMovable(true);
    drawing->setFloatable(true);
    drawing->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    for (const auto& label : {QStringLiteral("Select Objects"), QStringLiteral("Line"), QStringLiteral("Arrow"), QStringLiteral("Rectangle"),
                             QStringLiteral("Oval"), QStringLiteral("Text Box"), QStringLiteral("Freeform"), QStringLiteral("Filled Shapes"),
                             QStringLiteral("Rotate"), QStringLiteral("Line Color"), QStringLiteral("Fill Color"), QStringLiteral("Arrowhead")}) {
        addToolbarAction(drawing, label, label + " command");
    }
    m_parent->addToolBar(Qt::BottomToolBarArea, drawing);
}

void PowerPoint7MenuBar::createStatusWidgets()
{
    auto* statusBar = m_parent->statusBar();
    if (!statusBar) {
        return;
    }

    auto* slide = new QLabel("Slide 1", statusBar);
    auto* layout = new QLabel("Layout", statusBar);
    auto* zoom = new QLabel("100%", statusBar);
    slide->setStyleSheet("QLabel { padding: 0 6px; } ");
    layout->setStyleSheet("QLabel { padding: 0 6px; } ");
    zoom->setStyleSheet("QLabel { padding: 0 6px; } ");

    statusBar->addPermanentWidget(slide);
    statusBar->addPermanentWidget(layout);
    statusBar->addPermanentWidget(zoom);
}


} // namespace blastmaster::presentation
