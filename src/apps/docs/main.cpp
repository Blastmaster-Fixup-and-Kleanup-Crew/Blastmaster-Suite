#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>

#include "Word7MenuBar.h"
#include "Document.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Docs");
    app.setApplicationDisplayName("Blastmaster Docs");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Blastmaster");
    app.setOrganizationDomain("blastmaster.local");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Docs - Untitled");
    window.setAccessibleName("Blastmaster Docs");

    auto* document = new blastmaster::docs::Document();

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* editor = new QTextEdit(central);
    editor->setObjectName("documentEditor");
    editor->setAccessibleName("Document editor");
    editor->setAccessibleDescription("Main editing area for the current Blastmaster Docs document.");
    editor->setTabChangesFocus(false);
    editor->setPlaceholderText("Start typing your document...");
    editor->setStyleSheet("QPlainTextEdit { font-family: 'Arial'; font-size: 11px; padding: 8px; }");
    layout->addWidget(editor);

    QObject::connect(editor, &QPlainTextEdit::textChanged, [document, editor]() {
        document->setContent(editor->toHtml());
    });

    window.setCentralWidget(central);

    blastmaster::docs::Word7MenuBar word7MenuBar(&window, document);
    window.setMenuBar(word7MenuBar.menuBar());

    window.statusBar()->showMessage("Ready");
    window.show();
    editor->setFocus();

    return app.exec();
}
