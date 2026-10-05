#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>

namespace blastmaster::docs {

/**
 * @class Word7MenuBar
 * @brief Implements Microsoft Word 95 (Word 7.0) classic menu bar
 * 
 * Provides traditional top menu layout with nine core drop-down menus:
 * File, Edit, View, Insert, Format, Tools, Table, Window, Help
 */
class Word7MenuBar {
public:
    explicit Word7MenuBar(QMainWindow* parent);
    
    QMenuBar* menuBar() const { return m_menuBar; }
    
private:
    QMainWindow* m_parent;
    QMenuBar* m_menuBar;
    
    // Menu instances
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
};

} // namespace blastmaster::docs
