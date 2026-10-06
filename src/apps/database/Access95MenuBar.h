#pragma once

#include <QMenuBar>
#include <QMainWindow>

class QMenu;
class QToolBar;

namespace blastmaster::database {

class DatabaseDocument;

class Access95MenuBar {
public:
    explicit Access95MenuBar(QMainWindow* parent, DatabaseDocument* document);

    QMenuBar* menuBar() const { return m_menuBar; }

private:
    QMainWindow* m_parent;
    DatabaseDocument* m_document;
    QMenuBar* m_menuBar;

    QMenu* m_fileMenu{};
    QMenu* m_editMenu{};
    QMenu* m_viewMenu{};
    QMenu* m_insertMenu{};
    QMenu* m_formatMenu{};
    QMenu* m_toolsMenu{};
    QMenu* m_windowMenu{};
    QMenu* m_helpMenu{};

    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createInsertMenu();
    void createFormatMenu();
    void createToolsMenu();
    void createWindowMenu();
    void createHelpMenu();
    void createToolbars();
};

} // namespace blastmaster::database
