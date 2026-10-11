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
#include <QTableWidget>
#include <QColorDialog>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QTextDocument>

namespace blastmaster::database {

static QAction* action(QObject* parent, const QString& text,
                       const QKeySequence& shortcut = QKeySequence())
{
    auto* a = new QAction(text, parent);
    if (!shortcut.isEmpty()) a->setShortcut(shortcut);
    return a;
}

static QString tableAsHtml(DatabaseDocument* document, const QString& table)
{
    QString html = QStringLiteral("<!doctype html><html><head><meta charset=\"utf-8\"></head><body><h1>%1</h1><table border=\"1\" cellspacing=\"0\" cellpadding=\"4\"><thead><tr>")
        .arg(table.toHtmlEscaped());
    for (const QString& column : document->columns(table))
        html += QStringLiteral("<th>%1</th>").arg(column.toHtmlEscaped());
    html += QStringLiteral("</tr></thead><tbody>");
    for (const QVariantList& row : document->records(table)) {
        html += QStringLiteral("<tr>");
        for (const QVariant& value : row)
            html += QStringLiteral("<td>%1</td>").arg(value.toString().toHtmlEscaped());
        html += QStringLiteral("</tr>");
    }
    html += QStringLiteral("</tbody></table></body></html>");
    return html;
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
    auto* recentMenu = m_fileMenu->addMenu(QStringLiteral("Recent Databases"));
    QObject::connect(recentMenu, &QMenu::aboutToShow, [this, recentMenu] {
        recentMenu->clear();
        const auto recent = blastmaster::RecentFiles::files(QStringLiteral("databases"));
        if (recent.isEmpty()) {
            auto* empty = recentMenu->addAction(QStringLiteral("(No recent databases)"));
            empty->setEnabled(false);
        } else {
            for (const QString& path : recent) {
                auto* item = recentMenu->addAction(QFileInfo(path).fileName());
                item->setToolTip(path);
                QObject::connect(item, &QAction::triggered, [this, path] {
                    if (!m_document->openDatabase(path)) {
                        QMessageBox::critical(m_parent, QStringLiteral("Open Database"),
                                              QStringLiteral("Failed to open database:\n%1").arg(path));
                        return;
                    }
                    blastmaster::RecentFiles::add(QStringLiteral("databases"), path);
                    if (m_parent->statusBar())
                        m_parent->statusBar()->showMessage(QStringLiteral("Database opened: %1").arg(path), 3000);
                });
            }
            recentMenu->addSeparator();
            auto* clear = recentMenu->addAction(QStringLiteral("Clear Recent Databases"));
            QObject::connect(clear, &QAction::triggered, [recentMenu] {
                blastmaster::RecentFiles::clear(QStringLiteral("databases"));
                recentMenu->hide();
            });
        }
    });
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
            QStringLiteral("Blastmaster Databases (*.dbbx);;SQLite databases (*.db *.sqlite *.sqlite3);;All files (*.*)"));
        if (!path.isEmpty()) {
            if (m_document->openDatabase(path)) {
                blastmaster::RecentFiles::add(QStringLiteral("databases"), path);
                if (m_parent->statusBar())
                    m_parent->statusBar()->showMessage(QStringLiteral("Database opened: %1").arg(path), 2500);
            } else {
                QMessageBox::critical(m_parent, QStringLiteral("Open Database"),
                                      QStringLiteral("Failed to open database:\n%1").arg(path));
            }
        }
    });
    QObject::connect(aSave, &QAction::triggered, [this, aSaveAs] {
        if (m_document->filePath().isEmpty()) {
            aSaveAs->trigger();
        } else if (m_document->saveDatabase()) {
            blastmaster::RecentFiles::add(QStringLiteral("databases"), m_document->filePath());
            m_parent->statusBar()->showMessage(QStringLiteral("Database saved."), 2500);
        }
    });
    QObject::connect(aSaveAs, &QAction::triggered, [this] {
        const QString path = QFileDialog::getSaveFileName(
            m_parent, QStringLiteral("Save Database As"), QStringLiteral("Database.dbbx"),
            QStringLiteral("Blastmaster Databases (*.dbbx);;SQLite databases (*.db *.sqlite *.sqlite3);;All files (*.*)"));
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
        auto* selector = m_parent->findChild<QComboBox*>();
        const QString table = selector ? selector->currentText() : QString();
        if (table.isEmpty()) {
            QMessageBox::information(m_parent, QStringLiteral("Export HTML"),
                QStringLiteral("Select a table before exporting."));
            return;
        }
        const QString path = QFileDialog::getSaveFileName(m_parent,
            QStringLiteral("Export Table as HTML"), table + QStringLiteral(".html"),
            QStringLiteral("HTML files (*.html *.htm)"));
        if (path.isEmpty()) return;
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(m_parent, QStringLiteral("Export HTML"), file.errorString());
            return;
        }
        QTextStream out(&file);
        out << "<!doctype html>\n<html><head><meta charset=\"utf-8\"><title>"
            << table.toHtmlEscaped() << "</title></head><body><h1>" << table.toHtmlEscaped()
            << "</h1><table border=\"1\" cellspacing=\"0\" cellpadding=\"4\"><thead><tr>";
        const QStringList columns = m_document->columns(table);
        for (const QString& column : columns) out << "<th>" << column.toHtmlEscaped() << "</th>";
        out << "</tr></thead><tbody>";
        for (const QVariantList& row : m_document->records(table)) {
            out << "<tr>";
            for (const QVariant& value : row) out << "<td>" << value.toString().toHtmlEscaped() << "</td>";
            out << "</tr>";
        }
        out << "</tbody></table></body></html>";
        file.close();
        m_parent->statusBar()->showMessage(QStringLiteral("Exported %1 to HTML.").arg(table), 3000);
    });
    QObject::connect(aPage, &QAction::triggered, [this] {
        QPrinter printer(QPrinter::HighResolution);
        QPageSetupDialog dialog(&printer, m_parent);
        dialog.exec();
    });
    QObject::connect(aPreview, &QAction::triggered, [this] {
        auto* selector = m_parent->findChild<QComboBox*>();
        const QString table = selector ? selector->currentText() : QString();
        if (table.isEmpty()) {
            QMessageBox::information(m_parent, QStringLiteral("Print Preview"),
                QStringLiteral("Select a table before previewing."));
            return;
        }
        QPrinter printer(QPrinter::HighResolution);
        QPrintPreviewDialog preview(&printer, m_parent);
        QObject::connect(&preview, &QPrintPreviewDialog::paintRequested, [&printer, this, table](QPrinter* requestedPrinter) {
            QTextDocument document;
            document.setHtml(tableAsHtml(m_document, table));
            document.print(requestedPrinter ? requestedPrinter : &printer);
        });
        preview.exec();
    });
    QObject::connect(aPrint, &QAction::triggered, [this] {
        auto* selector = m_parent->findChild<QComboBox*>();
        const QString table = selector ? selector->currentText() : QString();
        if (table.isEmpty()) {
            QMessageBox::information(m_parent, QStringLiteral("Print"),
                QStringLiteral("Select a table before printing."));
            return;
        }
        QPrinter printer(QPrinter::HighResolution);
        QPrintDialog dialog(&printer, m_parent);
        if (dialog.exec() != QDialog::Accepted) return;
        QTextDocument document;
        document.setHtml(tableAsHtml(m_document, table));
        document.print(&printer);
    });
    QObject::connect(aSend, &QAction::triggered, [this] {
        QMessageBox::information(m_parent, QStringLiteral("Send"),
            QStringLiteral("Export the selected table to HTML or print it to share a copy."));
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
    auto* toolbarsMenu = m_viewMenu->addMenu(QStringLiteral("Toolbars"));
    // Toolbars are created after the menu bar constructor calls createViewMenu(),
    // so populate this submenu when it opens rather than during initialization.
    QObject::connect(toolbarsMenu, &QMenu::aboutToShow, [this, toolbarsMenu]() {
        toolbarsMenu->clear();
        const auto toolbars = m_parent->findChildren<QToolBar*>();
        if (toolbars.isEmpty()) {
            auto* empty = toolbarsMenu->addAction(QStringLiteral("(No toolbars available)"));
            empty->setEnabled(false);
            return;
        }
        for (QToolBar* toolbar : toolbars) {
            auto* toggle = toolbarsMenu->addAction(toolbar->windowTitle());
            toggle->setCheckable(true);
            toggle->setChecked(!toolbar->isHidden());
            QObject::connect(toggle, &QAction::toggled, toolbar, &QWidget::setVisible);
            QObject::connect(toolbar, &QToolBar::visibilityChanged, toggle, &QAction::setChecked);
        }
    });
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
    auto* helpTopics = action(m_parent, QStringLiteral("Blastmaster Databases Help &Topics"), QKeySequence::HelpContents);
    m_helpMenu->addAction(helpTopics);
    QObject::connect(helpTopics, &QAction::triggered, [this] {
        blastmaster::HelpTopicsWindow::showFor(m_parent, QStringLiteral("databases"));
    });
    auto* assistance = action(m_parent, QStringLiteral("Getting Assistance"));
    m_helpMenu->addAction(assistance);
    QObject::connect(assistance, &QAction::triggered, [this] {
        blastmaster::HelpTopicsWindow::showFor(m_parent, QStringLiteral("databases"));
    });
    auto* support = action(m_parent, QStringLiteral("Blastmaster Support"));
    m_helpMenu->addAction(support);
    QObject::connect(support, &QAction::triggered, [this] {
        blastmaster::HelpTopicsWindow::showFor(m_parent, QStringLiteral("databases"));
    });
    m_helpMenu->addAction(action(m_parent, QStringLiteral("&About Blastmaster Databases")));
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
    addToolbarAction(standard, QStringLiteral("Open"), QKeySequence::Open);
    addToolbarAction(standard, QStringLiteral("Save"), QKeySequence::Save);
    addToolbarAction(standard, QStringLiteral("Print"));
    addToolbarAction(standard, QStringLiteral("Print Preview"));
    standard->addSeparator();
    addToolbarAction(standard, QStringLiteral("Cut"), QKeySequence::Cut);
    addToolbarAction(standard, QStringLiteral("Copy"), QKeySequence::Copy);
    addToolbarAction(standard, QStringLiteral("Paste"), QKeySequence::Paste);
    addToolbarAction(standard, QStringLiteral("Format Painter"));
    standard->addSeparator();
    addToolbarAction(standard, QStringLiteral("Undo"), QKeySequence::Undo);
    addToolbarAction(standard, QStringLiteral("Spelling"));
    addToolbarAction(standard, QStringLiteral("Relationships"));
    addToolbarAction(standard, QStringLiteral("Database Window"));
    addToolbarAction(standard, QStringLiteral("Table Wizard"));
    addToolbarAction(standard, QStringLiteral("Query Wizard"));
    addToolbarAction(standard, QStringLiteral("Form Wizard"));
    addToolbarAction(standard, QStringLiteral("Report Wizard"));
    m_parent->addToolBar(Qt::TopToolBarArea, standard);
    standard->show();

    auto* formatting = new QToolBar(QStringLiteral("Formatting"), m_parent);
    formatting->setObjectName(QStringLiteral("toolbar_formatting"));
    formatting->setMovable(true);
    formatting->setFloatable(true);

    auto selectedCells = [this]() {
        QList<QTableWidgetItem*> items;
        auto* grid = m_parent->findChild<QTableWidget*>(QStringLiteral("databaseGrid"));
        if (!grid) return items;
        items = grid->selectedItems();
        if (items.isEmpty() && grid->currentItem()) items.append(grid->currentItem());
        return items;
    };

    auto* font = new QFontComboBox(formatting);
    font->setCurrentFont(QFont(QStringLiteral("MS Sans Serif"), 8));
    formatting->addWidget(font);
    QObject::connect(font, &QFontComboBox::currentFontChanged, [this, selectedCells](const QFont& chosen) {
        for (auto* item : selectedCells()) {
            QFont cellFont = item->font();
            cellFont.setFamily(chosen.family());
            item->setFont(cellFont);
        }
        if (m_parent->statusBar())
            m_parent->statusBar()->showMessage(QStringLiteral("Font: %1").arg(chosen.family()), 2000);
    });

    auto* size = new QComboBox(formatting);
    for (int pointSize : {8, 9, 10, 11, 12, 14, 16, 18, 20, 24, 28, 36})
        size->addItem(QString::number(pointSize));
    size->setCurrentText(QStringLiteral("8"));
    formatting->addWidget(size);
    QObject::connect(size, &QComboBox::currentTextChanged, [this, selectedCells](const QString& value) {
        bool ok = false;
        const int pointSize = value.toInt(&ok);
        if (ok) for (auto* item : selectedCells()) {
            QFont cellFont = item->font();
            cellFont.setPointSize(pointSize);
            item->setFont(cellFont);
        }
    });

    for (const QString& text : {QStringLiteral("Bold"), QStringLiteral("Italic"),
                                QStringLiteral("Underline"), QStringLiteral("Align Left"),
                                QStringLiteral("Center"), QStringLiteral("Align Right"),
                                QStringLiteral("Font Color")}) {
        auto* formatAction = addToolbarAction(formatting, text);
        QObject::connect(formatAction, &QAction::triggered, [this, text, selectedCells] {
            const auto items = selectedCells();
            for (auto* item : items) {
                QFont cellFont = item->font();
                if (text == QStringLiteral("Bold")) cellFont.setBold(!cellFont.bold());
                else if (text == QStringLiteral("Italic")) cellFont.setItalic(!cellFont.italic());
                else if (text == QStringLiteral("Underline")) cellFont.setUnderline(!cellFont.underline());
                else if (text == QStringLiteral("Align Left")) item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                else if (text == QStringLiteral("Center")) item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                else if (text == QStringLiteral("Align Right")) item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                else if (text == QStringLiteral("Font Color")) {
                    const QColor color = QColorDialog::getColor(item->foreground().color(), m_parent,
                                                                  QStringLiteral("Cell Font Color"));
                    if (color.isValid()) item->setForeground(color);
                }
                if (text == QStringLiteral("Bold") || text == QStringLiteral("Italic") ||
                    text == QStringLiteral("Underline")) item->setFont(cellFont);
            }
            if (items.isEmpty() && m_parent->statusBar())
                m_parent->statusBar()->showMessage(QStringLiteral("Select one or more cells first."), 2500);
        });
    }

    m_parent->addToolBar(Qt::TopToolBarArea, formatting);
    formatting->show();
}

} // namespace blastmaster::database
