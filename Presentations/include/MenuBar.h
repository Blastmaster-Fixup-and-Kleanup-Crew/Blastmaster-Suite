#ifndef PRESENTATIONS_MENUBAR_H
#define PRESENTATIONS_MENUBAR_H

#include <string>
#include <vector>
#include <functional>

namespace Presentations {

// Menu item callback type
using MenuItemCallback = std::function<void()>;

// Represents a single menu item
struct MenuItem {
    std::string label;
    MenuItemCallback callback;
    bool separator = false;
    
    MenuItem(const std::string& lbl, MenuItemCallback cb = nullptr)
        : label(lbl), callback(cb), separator(false) {}
    
    MenuItem(const std::string& lbl, bool isSeparator)
        : label(lbl), separator(isSeparator) {}
};

// Represents a top-level menu
class Menu {
public:
    Menu(const std::string& title);
    
    void AddItem(const MenuItem& item);
    void AddSeparator();
    
    const std::string& GetTitle() const { return title; }
    const std::vector<MenuItem>& GetItems() const { return items; }
    
private:
    std::string title;
    std::vector<MenuItem> items;
};

// Main menu bar containing all top-level menus
class MenuBar {
public:
    MenuBar();
    
    void InitializeFileMenu();
    void InitializeEditMenu();
    void InitializeViewMenu();
    void InitializeInsertMenu();
    void InitializeFormatMenu();
    void InitializeToolsMenu();
    void InitializeSlideShowMenu();
    void InitializeWindowMenu();
    void InitializeHelpMenu();
    
    void AddMenu(const Menu& menu);
    const std::vector<Menu>& GetMenus() const { return menus; }
    
private:
    std::vector<Menu> menus;
};

} // namespace Presentations

#endif // PRESENTATIONS_MENUBAR_H
