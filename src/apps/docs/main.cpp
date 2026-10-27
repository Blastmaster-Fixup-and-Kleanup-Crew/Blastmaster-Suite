#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>

#include "../Office95Style.h"
#include "../Office95UnsavedChangesGuard.h"
#include <QFileDialog>
#include "Word7MenuBar.h"
#include "Document.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Docs");
    const QIcon docsIcon(QStringLiteral(":/docs/icons/docs.ico"));
    app.setWindowIcon(docsIcon);
    app.setApplicationDisplayName("Blastmaster Docs");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Blastmaster");
    app.setOrganizationDomain("blastmaster.local");
    blastmaster::ui::applyOffice95Style(app);

    QMainWindow window;
    window.setWindowIcon(docsIcon);
    window.resize(900, 650);
    window.setWindowTitle("Blastmaster Docs - Untitled");
    window.setAccessibleName("Blastmaster Docs");

    auto* document = new blastmaster::docs::Document();
    if (argc > 1) {
        document->setFilePath(QString::fromLocal8Bit(argv[1]));
        document->load();
    }

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* editor = new QTextEdit(central);
    editor->setObjectName("documentEditor");
    editor->setAccessibleName("Document editor");
    editor->setAccessibleDescription("Main editing area for the current Blastmaster Docs document.");
    editor->setTabChangesFocus(false);
    editor->setPlaceholderText("Start typing your document...");
    editor->setHtml(document->content());
    editor->setStyleSheet("QTextEdit { font-family: 'Times New Roman'; font-size: 12pt; padding: 16px; }");
    layout->addWidget(editor);

    QObject::connect(editor, &QTextEdit::textChanged, [document, editor]() {
        document->setContent(editor->toHtml());
    });

    window.setCentralWidget(central);

    blastmaster::docs::Word7MenuBar word7MenuBar(&window, document);
    window.setMenuBar(word7MenuBar.menuBar());
    new blastmaster::ui::UnsavedChangesGuard(&window,
        [document]() { return document->isDirty(); },
        [&window, document]() {
            QString path = document->filePath();
            if (path.isEmpty())
                path = QFileDialog::getSaveFileName(&window, QStringLiteral("Save Document"),
                    QStringLiteral("Document.dccx"), blastmaster::docs::Document::fileFilter());
            if (path.isEmpty()) return false;
            return document->saveAs(path);
        });

    window.statusBar()->showMessage("Ready");
    window.show();
    editor->setFocus();

    return app.exec();
}
