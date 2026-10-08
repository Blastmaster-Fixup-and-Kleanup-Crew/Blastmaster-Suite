#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include "PowerPoint7MenuBar.h"
#include "PresentationDocument.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Presentations");
    app.setApplicationDisplayName("Blastmaster Presentations");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Blastmaster");
    app.setOrganizationDomain("blastmaster.local");

    QMainWindow window;
    window.resize(1100, 700);
    window.setWindowTitle("Blastmaster Presentations - Untitled");
    window.setAccessibleName("Blastmaster Presentations");

    auto* document = new blastmaster::presentation::PresentationDocument();

    auto* central = new QWidget(&window);
    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(4, 4, 4, 4);

    auto* slides = new QListWidget(central);
    slides->setObjectName("slideList");
    slides->setAccessibleName("Slide list");
    slides->setAccessibleDescription("List of slides in the current presentation. Use Up and Down to change slides.");
    slides->setFixedWidth(180);
    slides->setAlternatingRowColors(true);

    auto* editorPane = new QWidget(central);
    auto* editorLayout = new QVBoxLayout(editorPane);
    auto* titleEdit = new QLineEdit(editorPane);
    titleEdit->setObjectName("slideTitle");
    titleEdit->setAccessibleName("Slide title");
    titleEdit->setPlaceholderText("Slide title");
    auto* bodyEdit = new QPlainTextEdit(editorPane);
    bodyEdit->setObjectName("slideBody");
    bodyEdit->setAccessibleName("Slide body");
    bodyEdit->setAccessibleDescription("Main text content for the selected slide.");
    bodyEdit->setPlaceholderText("Click to add text");
    editorLayout->addWidget(titleEdit);
    editorLayout->addWidget(bodyEdit);

    layout->addWidget(slides);
    layout->addWidget(editorPane, 1);
    window.setCentralWidget(central);

    auto refresh = [&]() {
        slides->clear();
        for (int i = 0; i < document->slideCount(); ++i)
            slides->addItem(QString("%1. %2").arg(i + 1).arg(document->slide(i).title));
        if (slides->count() > 0 && slides->currentRow() < 0)
            slides->setCurrentRow(0);
    };

    auto loadSlide = [&]() {
        const int row = slides->currentRow();
        if (row < 0 || row >= document->slideCount()) return;
        const auto& slide = document->slide(row);
        titleEdit->setText(slide.title);
        bodyEdit->setPlainText(slide.body);
        window.statusBar()->showMessage(QString("Slide %1 of %2").arg(row + 1).arg(document->slideCount()));
    };

    QObject::connect(slides, &QListWidget::currentRowChanged, [&](int) { loadSlide(); });
    QObject::connect(titleEdit, &QLineEdit::textChanged, [&](const QString& text) {
        const int row = slides->currentRow();
        if (row >= 0 && row < document->slideCount())
            document->setSlide(row, text, bodyEdit->toPlainText());
        if (row >= 0 && row < slides->count())
            slides->item(row)->setText(QString("%1. %2").arg(row + 1).arg(text));
    });
    QObject::connect(bodyEdit, &QPlainTextEdit::textChanged, [&]() {
        const int row = slides->currentRow();
        if (row >= 0 && row < document->slideCount())
            document->setSlide(row, titleEdit->text(), bodyEdit->toPlainText());
    });

    refresh();
    loadSlide();

    blastmaster::presentation::PowerPoint7MenuBar menuBar(&window, document);
    window.setMenuBar(menuBar.menuBar());

    window.statusBar()->showMessage("Ready");
    window.show();
    slides->setFocus();
    return app.exec();
}
