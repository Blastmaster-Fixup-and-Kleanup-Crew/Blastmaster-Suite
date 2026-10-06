#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>
#include <memory>

namespace blastmaster::docs {

class Document;

/**
 * @class Word7MenuBar
 * @brief Implements Microsoft Word 95 (Word 7.0) classic menu bar
 * 
 * Provides traditional top menu layout with nine core drop-down menus:
 * File, Edit, View, Insert, Format, Tools, Table, Window, Help
 */
class Word7MenuBar {
public:
    explicit Word7MenuBar(QMainWindow* parent, Document* document);
    
    QMenuBar* menuBar() const { return m_menuBar; }
    
private:
    QMainWindow* m_parent;
    Document* m_document;
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
    void createToolbars();
    void createRuler();
    void wireAction(QAction* action, const QString& label);
    
    // File menu handlers
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileExit();
};

} // namespace blastmaster::docs
