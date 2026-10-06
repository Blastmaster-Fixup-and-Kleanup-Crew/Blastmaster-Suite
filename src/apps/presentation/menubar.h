#ifndef PRESENTATION_MENUBAR_H
#define PRESENTATION_MENUBAR_H

#include <string>
#include <vector>
#include <functional>

namespace presentation {

/**
 * @brief Menu Item structure for PowerPoint-like presentation application
 * Implements Microsoft PowerPoint 7.0 (Windows 95) menu structure
 */
struct MenuItem {
    std::string label;
    std::string shortcut;
    std::function<void()> action;
    bool enabled = true;
    bool checked = false;
};

/**
 * @brief File Menu - handles file operations
 */
class FileMenu {
public:
    FileMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onNew();
    void onOpen();
    void onClose();
    void onSave();
    void onSaveAs();
    void onPackAndGo();
    void onPageSetup();
    void onPrintSetup();
    void onPrint();
    void onSend();
    void onProperties();
};

/**
 * @brief Edit Menu - handles editing operations
 */
class EditMenu {
public:
    EditMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onPasteSpecial();
    void onClear();
    void onSelectAll();
    void onDuplicate();
    void onDeleteSlide();
    void onFind();
    void onReplace();
    void onObject();
};

/**
 * @brief View Menu - handles view options
 */
class ViewMenu {
public:
    ViewMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onNormal();
    void onSlideView();
    void onOutlineView();
    void onSlideSorter();
    void onNotesPage();
    void onSlideShow();
    void onBlackAndWhite();
    void onToolbars();
    void onRuler();
    void onGuides();
    void onZoom();
};

/**
 * @brief Insert Menu - handles slide and content insertion
 */
class InsertMenu {
public:
    InsertMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onNewSlide();
    void onDuplicateSlide();
    void onSlideNumbers();
    void onDateAndTime();
    void onSymbol();
    void onObject();
    void onMovieAndSound();
    void onExcelWorksheet();
};

/**
 * @brief Format Menu - handles text and slide formatting
 */
class FormatMenu {
public:
    FormatMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onFont();
    void onBullet();
    void onAlignment();
    void onLineSpacing();
    void onChangeCase();
    void onTextDirection();
    void onSlideColorScheme();
    void onSlideBackground();
    void onApplyDesignTemplate();
};

/**
 * @brief Tools Menu - handles utilities and settings
 */
class ToolsMenu {
public:
    ToolsMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onSpelling();
    void onStyleChecker();
    void onAutoContentWizard();
    void onMeetingMinder();
    void onOptions();
    void onAddIns();
    void onMacro();
};

/**
 * @brief Slide Show Menu - handles presentation execution
 */
class SlideShowMenu {
public:
    SlideShowMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onViewShow();
    void onRehearseTimings();
    void onSlideTransition();
    void onHideSlide();
    void onSetUpShow();
};

/**
 * @brief Window Menu - handles window management
 */
class WindowMenu {
public:
    WindowMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onNewWindow();
    void onCascade();
    void onTile();
    void onArrangeIcons();
};

/**
 * @brief Help Menu - handles help and documentation
 */
class HelpMenu {
public:
    HelpMenu();
    
    std::vector<MenuItem> getItems() const { return items; }
    
private:
    std::vector<MenuItem> items;
    
    void initializeItems();
    void onContentsAndIndex();
    void onGettingAssistance();
    void onMicrosoftOnTheWeb();
    void onAbout();
};

/**
 * @brief Main Menu Bar for PowerPoint-like presentation application
 */
class MenuBar {
public:
    MenuBar();
    
    FileMenu& getFileMenu() { return fileMenu; }
    EditMenu& getEditMenu() { return editMenu; }
    ViewMenu& getViewMenu() { return viewMenu; }
    InsertMenu& getInsertMenu() { return insertMenu; }
    FormatMenu& getFormatMenu() { return formatMenu; }
    ToolsMenu& getToolsMenu() { return toolsMenu; }
    SlideShowMenu& getSlideShowMenu() { return slideShowMenu; }
    WindowMenu& getWindowMenu() { return windowMenu; }
    HelpMenu& getHelpMenu() { return helpMenu; }
    
    std::vector<std::string> getMenuLabels() const {
        return {"File", "Edit", "View", "Insert", "Format", "Tools", "Slide Show", "Window", "Help"};
    }
    
private:
    FileMenu fileMenu;
    EditMenu editMenu;
    ViewMenu viewMenu;
    InsertMenu insertMenu;
    FormatMenu formatMenu;
    ToolsMenu toolsMenu;
    SlideShowMenu slideShowMenu;
    WindowMenu windowMenu;
    HelpMenu helpMenu;
};

} // namespace presentation

#endif // PRESENTATION_MENUBAR_H
