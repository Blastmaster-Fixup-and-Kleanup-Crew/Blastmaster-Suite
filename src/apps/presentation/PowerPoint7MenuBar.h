#pragma once

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <functional>
#include <utility>

namespace blastmaster::presentation {

class PresentationDocument;

class PowerPoint7MenuBar {
public:
    explicit PowerPoint7MenuBar(QMainWindow* parent, PresentationDocument* document);

    QMenuBar* menuBar() const { return m_menuBar; }
    void setDocumentLoadedCallback(std::function<void()> callback) { m_documentLoadedCallback = std::move(callback); }

private:
    QMainWindow* m_parent;
    PresentationDocument* m_document;
    QMenuBar* m_menuBar;
    std::function<void()> m_documentLoadedCallback;

    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_insertMenu;
    QMenu* m_formatMenu;
    QMenu* m_toolsMenu;
    QMenu* m_slideShowMenu;
    QMenu* m_windowMenu;
    QMenu* m_helpMenu;

    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createInsertMenu();
    void createFormatMenu();
    void createToolsMenu();
    void createSlideShowMenu();
    void createWindowMenu();
    void createHelpMenu();
    void createToolbars();
    void createStatusWidgets();
    void wireAction(QAction* action, const QString& label);

    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileExit();
};

} // namespace blastmaster::presentation
