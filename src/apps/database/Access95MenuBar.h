#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>
#include <memory>

namespace blastmaster::database {

class DatabaseDocument;

/**
 * @class Access95MenuBar
 * @brief Implements Microsoft Access 95 (Access 7.0) classic menu bar
 * 
 * Provides traditional top menu layout with nine core drop-down menus:
 * File, Edit, View, Insert, Tools, Window, Help
 * 
 * Follows the classic Microsoft Access 1995 menu structure with commands
 * mapped to traditional DoCmd operations and VBA function equivalents.
 */
class Access95MenuBar {
public:
    explicit Access95MenuBar(QMainWindow* parent, DatabaseDocument* document);
    
    QMenuBar* menuBar() const { return m_menuBar; }
    
private:
    QMainWindow* m_parent;
    DatabaseDocument* m_document;
    QMenuBar* m_menuBar;
    
    // Menu instances
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_insertMenu;
    QMenu* m_toolsMenu;
    QMenu* m_windowMenu;
    QMenu* m_helpMenu;
    
    // File menu submenus
    QMenu* m_getExternalDataMenu;
    QMenu* m_officeLinksMenu;
    QMenu* m_analyzeMenu;
    QMenu* m_databaseUtilitiesMenu;
    QMenu* m_securityMenu;
    
    // View menu submenus
    QMenu* m_windowArrangeMenu;
    
    // Edit menu submenus
    QMenu* m_goToMenu;
    QMenu* m_linksMenu;
    
    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createInsertMenu();
    void createToolsMenu();
    void createWindowMenu();
    void createHelpMenu();
    
    // File menu handlers
    void onFileNewDatabase();
    void onFileOpenDatabase();
    void onFileGetExternalDataImport();
    void onFileGetExternalDataLink();
    void onFileClose();
    void onFileSave();
    void onFileSaveAs();
    void onFileExport();
    void onFileDatabaseProperties();
    void onFilePageSetup();
    void onFilePrintPreview();
    void onFilePrint();
    void onFileSend();
    void onFileExit();
    
    // Edit menu handlers
    void onEditUndo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditPasteSpecial();
    void onEditDelete();
    void onEditDeleteRecord();
    void onEditSelectAll();
    void onEditSelectRecord();
    void onEditFind();
    void onEditReplace();
    void onEditGoToFirst();
    void onEditGoToPrevious();
    void onEditGoToNext();
    void onEditGoToLast();
    void onEditGoToNewRecord();
    void onEditOLELinks();
    
    // View menu handlers
    void onViewDesignView();
    void onViewDatasheetView();
    void onViewFormView();
    void onViewToolbars();
    void onViewIndexes();
    void onViewRelationships();
    void onViewSortingAndGrouping();
    void onViewCode();
    void onViewProperties();
    
    // Insert menu handlers
    void onInsertTable();
    void onInsertQuery();
    void onInsertForm();
    void onInsertReport();
    void onInsertMacro();
    void onInsertModule();
    void onInsertAutoForm();
    void onInsertAutoReport();
    void onInsertField();
    void onInsertRecord();
    void onInsertActiveXControl();
    
    // Tools menu handlers
    void onToolsSpellCheck();
    void onToolsAutoCorrect();
    void onToolsOfficeLinks();
    void onToolsRelationships();
    void onToolsAnalyzeTable();
    void onToolsAnalyzePerformance();
    void onToolsAnalyzeDocumenter();
    void onToolsCompactDatabase();
    void onToolsRepairDatabase();
    void onToolsConvertDatabase();
    void onToolsSetPassword();
    void onToolsWorkgroupAdministrator();
    void onToolsPermissions();
    void onToolsCreateReplica();
    void onToolsSynchronizeNow();
    void onToolsStartup();
    void onToolsMacro();
    void onToolsOptions();
    
    // Window menu handlers
    void onWindowTileHorizontally();
    void onWindowTileVertically();
    void onWindowCascade();
    void onWindowHide();
    void onWindowUnhide();
    void onWindowSizeToFitForm();
    
    // Help menu handlers
    void onHelpTopics();
    void onHelpAnswerWizard();
    void onHelpAbout();
};

} // namespace blastmaster::database
