#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QPlainTextEdit>

#include "Word7MenuBar.h"
#include "Document.h"
#include "blastmaster/ProductKeyValidator.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Docs");
    app.setApplicationDisplayName("Blastmaster Docs");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Docs - Untitled");

    // Create the document model
    auto* document = new blastmaster::docs::Document(&window);

    // Create central widget with text editor
    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* editor = new QPlainTextEdit(central);
    editor->setPlaceholderText("Start typing your document...");
    editor->setStyleSheet("QPlainTextEdit { font-family: 'Arial'; font-size: 11px; padding: 8px; }");
    layout->addWidget(editor);

    // Wire document content to editor
    QObject::connect(editor, &QPlainTextEdit::textChanged, [document, editor]() {
        document->setContent(editor->toPlainText());
    });

    window.setCentralWidget(central);

    // Create Word 7.0 menu bar and wire it to the document
    blastmaster::docs::Word7MenuBar word7MenuBar(&window, document);
    window.setMenuBar(word7MenuBar.menuBar());

    // Setup status bar
    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Standard);
    const std::string sample_key = "BMSSTD-2026-WORD";
    const bool valid = validator.validate(sample_key);
    
    QString statusMsg = QString("Edition: %1 | Valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No");
    window.statusBar()->showMessage(statusMsg);

    window.show();

    return app.exec();
}
