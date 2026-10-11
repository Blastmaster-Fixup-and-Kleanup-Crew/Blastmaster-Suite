#pragma once

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>

// Shared visual baseline for the four Blastmaster Suite applications.
// Prefer a square, high-contrast Windows 95-era desktop over modern rounded UI.
namespace blastmaster::ui {

inline void applyOffice95Style(QApplication& app)
{
    if (QStyle* classic = QStyleFactory::create(QStringLiteral("Windows"))) {
        app.setStyle(classic);
    } else if (QStyle* fallback = QStyleFactory::create(QStringLiteral("Fusion"))) {
        app.setStyle(fallback);
    }

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(192, 192, 192));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    palette.setColor(QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Button, QColor(192, 192, 192));
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    palette.setColor(QPalette::Highlight, QColor(0, 0, 128));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(0, 0, 128));
    palette.setColor(QPalette::Mid, QColor(128, 128, 128));
    palette.setColor(QPalette::Dark, QColor(64, 64, 64));
    palette.setColor(QPalette::Light, QColor(255, 255, 255));
    app.setPalette(palette);

    QFont classicFont(QStringLiteral("MS Sans Serif"), 8);
    if (!classicFont.exactMatch())
        classicFont = QFont(QStringLiteral("Arial"), 8);
    app.setFont(classicFont);

    app.setStyleSheet(QStringLiteral(R"(
        QMainWindow, QWidget {
            color: #000000;
        }
        QMenuBar {
            background: #c0c0c0;
            color: #000000;
            border: 0;
            spacing: 2px;
        }
        QMenuBar::item {
            background: transparent;
            padding: 3px 7px;
            margin: 1px;
        }
        QMenuBar::item:selected, QMenuBar::item:pressed {
            background: #000080;
            color: #ffffff;
        }
        QMenu {
            background: #c0c0c0;
            color: #000000;
            border: 1px solid #404040;
            padding: 2px;
        }
        QMenu::item {
            padding: 4px 24px 4px 24px;
        }
        QMenu::item:selected {
            background: #000080;
            color: #ffffff;
        }
        QToolBar {
            background: #c0c0c0;
            border: 0;
            spacing: 2px;
            padding: 2px;
        }
        QToolButton {
            background: #c0c0c0;
            border: 1px solid transparent;
            padding: 3px;
            min-width: 20px;
            min-height: 20px;
        }
        QToolButton:hover {
            border-top: 1px solid #ffffff;
            border-left: 1px solid #ffffff;
            border-right: 1px solid #404040;
            border-bottom: 1px solid #404040;
        }
        QPushButton {
            background: #c0c0c0;
            color: #000000;
            border-top: 2px solid #ffffff;
            border-left: 2px solid #ffffff;
            border-right: 2px solid #404040;
            border-bottom: 2px solid #404040;
            border-radius: 0;
            padding: 3px 9px;
            min-height: 20px;
        }
        QPushButton:pressed {
            border-top: 2px solid #404040;
            border-left: 2px solid #404040;
            border-right: 2px solid #ffffff;
            border-bottom: 2px solid #ffffff;
        }
        QLineEdit, QTextEdit, QPlainTextEdit, QTableWidget, QTableView,
        QListWidget, QComboBox {
            background: #ffffff;
            color: #000000;
            border: 1px solid #404040;
            border-radius: 0;
            selection-background-color: #000080;
            selection-color: #ffffff;
        }
        QTabWidget::pane {
            border: 1px solid #808080;
            background: #c0c0c0;
        }
        QTabBar::tab {
            background: #c0c0c0;
            color: #000000;
            border-top: 1px solid #ffffff;
            border-left: 1px solid #ffffff;
            border-right: 1px solid #404040;
            padding: 3px 8px;
            margin-right: 2px;
        }
        QStatusBar {
            background: #c0c0c0;
            color: #000000;
        }
        QStatusBar::item {
            border: 1px solid #808080;
        }
        QHeaderView::section {
            background: #c0c0c0;
            color: #000000;
            border-top: 1px solid #ffffff;
            border-left: 1px solid #ffffff;
            border-right: 1px solid #808080;
            border-bottom: 1px solid #808080;
            padding: 3px;
        }
    )"));
}

} // namespace blastmaster::ui
