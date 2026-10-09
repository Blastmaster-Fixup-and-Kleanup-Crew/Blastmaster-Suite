#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>
#include <memory>
#include <functional>

namespace blastmaster::spreadsheet {

class Workbook;

/**
 * Excel7MenuBar
 * Implements a classic Excel 7 (Excel 95/7.0 era) menu bar and provides helper
 * to create standard/formatting toolbars. Menus are lightweight and wired to
 * the provided Workbook model where appropriate.
 */
class Excel7MenuBar {
public:
    explicit Excel7MenuBar(QMainWindow* parent, Workbook* workbook);
    QMenuBar* menuBar() const { return m_menuBar; }
    void setDocumentLoadedCallback(std::function<void()> callback) { m_documentLoadedCallback = std::move(callback); }

private:
    QMainWindow* m_parent;
    Workbook* m_workbook;
    QMenuBar* m_menuBar;
    std::function<void()> m_documentLoadedCallback;

    // menus
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_insertMenu;
    QMenu* m_formatMenu;
    QMenu* m_toolsMenu;
    QMenu* m_dataMenu;
    QMenu* m_windowMenu;
    QMenu* m_helpMenu;

    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createInsertMenu();
    void createFormatMenu();
    void createToolsMenu();
    void createDataMenu();
    void createWindowMenu();
    void createHelpMenu();

    void createToolbars();
};

} // namespace blastmaster::spreadsheet
