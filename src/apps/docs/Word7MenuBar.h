#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>

namespace blastmaster::docs {

class Word7MenuBar {
public:
    explicit Word7MenuBar(QMainWindow* parent);
    
    QMenuBar* menuBar() const { return m_menuBar; }
    
private:
    QMainWindow* m_parent;
    QMenuBar* m_menuBar;
    
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_insertMenu;
    QMenu* m_formatMenu;
    QMenu* m_toolsMenu;
    QMenu* m_tableMenu;
    QMenu* m_windowMenu;
    QMenu* m_helpMenu;
    
    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createInsertMenu();
    void createFormatMenu();
    void createToolsMenu();
    void createTableMenu();
    void createWindowMenu();
    void createHelpMenu();
    void createToolbars();
    void createRuler();
    void wireAction(QAction* action, const QString& label);
};

} // namespace blastmaster::docs
