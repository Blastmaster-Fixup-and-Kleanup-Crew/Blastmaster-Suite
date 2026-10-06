#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QFontComboBox>
#include <QComboBox>
#include <QKeySequence>

#include "blastmaster/ProductKeyValidator.h"

static QAction* makeAction(const QString& text, const QKeySequence& shortcut = QKeySequence(), QObject* parent = nullptr) {
    QAction* a = new QAction(text, parent);
    if (!shortcut.isEmpty()) a->setShortcut(shortcut);
    return a;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Blastmaster Workbooks");
    app.setApplicationDisplayName("Blastmaster Workbooks");

    QMainWindow window;
    window.resize(1200, 800);
    window.setWindowTitle("Blastmaster Workbooks");

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);

    auto* title = new QLabel("Blastmaster Workbooks", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: 600; margin-top: 24px;");
    layout->addWidget(title);

    auto* body = new QLabel("Data analysis and calculation workspace ready.", central);
    body->setAlignment(Qt::AlignCenter);
    body->setStyleSheet("font-size: 16px; margin: 12px;");
    layout->addWidget(body);

    blastmaster::ProductKeyValidator validator(blastmaster::Edition::Standard);
    const std::string sample_key = "BMSSTD-SPREADSHEET";
    const bool valid = validator.validate(sample_key);
    auto* status = new QLabel(QString("Edition: %1 | Sample key valid: %2")
        .arg(QString::fromStdString(validator.edition_name()))
        .arg(valid ? "Yes" : "No"), central);
    status->setAlignment(Qt::AlignCenter);
    status->setStyleSheet("font-size: 12px; color: #4d4d4d;");
    layout->addWidget(status);

    window.setCentralWidget(central);
    window.statusBar()->showMessage("Spreadsheet module initialized");

    auto* menubar = window.menuBar();

    auto* fileMenu = menubar->addMenu("&File");
    QAction* aNew = makeAction("&New", QKeySequence::New, &window);
    QAction* aOpen = makeAction("&Open...", QKeySequence::Open, &window);
    QAction* aClose = makeAction("C&lose", QKeySequence("Ctrl+W"), &window);
    QAction* aSave = makeAction("&Save", QKeySequence::Save, &window);
    QAction* aSaveAs = makeAction("Save &As...", QKeySequence::SaveAs, &window);
    QAction* aPageSetup = makeAction("Page Setup...", QKeySequence(), &window);
    QAction* aPrintArea = makeAction("Print Area", QKeySequence(), &window);
    QAction* aPrintPreview = makeAction("Print Preview", QKeySequence("Ctrl+Shift+P"), &window);
    QAction* aPrint = makeAction("&Print...", QKeySequence::Print, &window);
    QAction* aSend = makeAction("Send", QKeySequence(), &window);
    QAction* aProperties = makeAction("Properties", QKeySequence(), &window);
    QAction* aExit = makeAction("E&xit", QKeySequence::Quit, &window);

    fileMenu->addAction(aNew);
    fileMenu->addAction(aOpen);
    fileMenu->addAction(aClose);
    fileMenu->addSeparator();
    fileMenu->addAction(aSave);
    fileMenu->addAction(aSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(aPageSetup);
    fileMenu->addAction(aPrintArea);
    fileMenu->addAction(aPrintPreview);
    fileMenu->addAction(aPrint);
    fileMenu->addSeparator();
    fileMenu->addAction(aSend);
    fileMenu->addAction(aProperties);
    fileMenu->addSeparator();
    fileMenu->addAction(aExit);

    auto* editMenu = menubar->addMenu("&Edit");
    QAction* aUndo = makeAction("Undo", QKeySequence::Undo, &window);
    QAction* aRedo = makeAction("Repeat", QKeySequence::Redo, &window);
    QAction* aCut = makeAction("Cut", QKeySequence::Cut, &window);
    QAction* aCopy = makeAction("Copy", QKeySequence::Copy, &window);
    QAction* aPaste = makeAction("Paste", QKeySequence::Paste, &window);
    QAction* aPasteSpecial = makeAction("Paste Special...", QKeySequence(), &window);
    QAction* aFill = makeAction("Fill", QKeySequence(), &window);
    QAction* aClear = makeAction("Clear", QKeySequence(), &window);
    QAction* aDelete = makeAction("Delete", QKeySequence::Delete, &window);
    QAction* aDeleteSheet = makeAction("Delete Sheet", QKeySequence(), &window);
    QAction* aMoveOrCopySheet = makeAction("Move or Copy Sheet...", QKeySequence(), &window);
    QAction* aFind = makeAction("Find...", QKeySequence::Find, &window);
    QAction* aReplace = makeAction("Replace...", QKeySequence::Replace, &window);
    QAction* aGoTo = makeAction("Go To...", QKeySequence("Ctrl+G"), &window);

    editMenu->addAction(aUndo);
    editMenu->addAction(aRedo);
    editMenu->addSeparator();
    editMenu->addAction(aCut);
    editMenu->addAction(aCopy);
    editMenu->addAction(aPaste);
    editMenu->addAction(aPasteSpecial);
    editMenu->addSeparator();
    editMenu->addAction(aFill);
    editMenu->addAction(aClear);
    editMenu->addAction(aDelete);
    editMenu->addAction(aDeleteSheet);
    editMenu->addAction(aMoveOrCopySheet);
    editMenu->addSeparator();
    editMenu->addAction(aFind);
    editMenu->addAction(aReplace);
    editMenu->addAction(aGoTo);

    auto* viewMenu = menubar->addMenu("&View");
    viewMenu->addAction(makeAction("Normal", QKeySequence(), &window));
    viewMenu->addAction(makeAction("Page Break Preview", QKeySequence(), &window));
    viewMenu->addSeparator();
    viewMenu->addAction(makeAction("Toolbars...", QKeySequence(), &window));
    viewMenu->addAction(makeAction("Formula Bar", QKeySequence(), &window));
    viewMenu->addAction(makeAction("Status Bar", QKeySequence(), &window));
    viewMenu->addSeparator();
    viewMenu->addAction(makeAction("Header and Footer...", QKeySequence(), &window));
    viewMenu->addAction(makeAction("Zoom...", QKeySequence(), &window));

    auto* insertMenu = menubar->addMenu("&Insert");
    insertMenu->addAction(makeAction("Cells...", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Rows", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Columns", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Worksheet", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Chart...", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Macro", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Page Break", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Function...", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Name...", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Note", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Picture...", QKeySequence(), &window));
    insertMenu->addAction(makeAction("Object...", QKeySequence(), &window));

    auto* formatMenu = menubar->addMenu("&Format");
    formatMenu->addAction(makeAction("Cells...", QKeySequence(), &window));
    formatMenu->addAction(makeAction("Row", QKeySequence(), &window));
    formatMenu->addAction(makeAction("Column", QKeySequence(), &window));
    formatMenu->addAction(makeAction("Sheet", QKeySequence(), &window));
    formatMenu->addAction(makeAction("AutoFormat...", QKeySequence(), &window));
    formatMenu->addAction(makeAction("Conditional Formatting...", QKeySequence(), &window));
    formatMenu->addAction(makeAction("Style...", QKeySequence(), &window));

    auto* toolsMenu = menubar->addMenu("&Tools");
    toolsMenu->addAction(makeAction("Spelling...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("AutoCorrect...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Trace Precedents", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Auditing", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Goal Seek...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Scenarios...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Protection...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Add-Ins...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Macro...", QKeySequence(), &window));
    toolsMenu->addAction(makeAction("Options...", QKeySequence(), &window));

    auto* dataMenu = menubar->addMenu("&Data");
    dataMenu->addAction(makeAction("Sort...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Filter...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Form...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Subtotals...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Table...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Text to Columns...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Consolidate...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("PivotTable...", QKeySequence(), &window));
    dataMenu->addAction(makeAction("Group and Outline", QKeySequence(), &window));

    auto* windowMenu = menubar->addMenu("&Window");
    windowMenu->addAction(makeAction("New Window", QKeySequence(), &window));
    windowMenu->addAction(makeAction("Arrange...", QKeySequence(), &window));
    windowMenu->addAction(makeAction("Hide", QKeySequence(), &window));
    windowMenu->addAction(makeAction("Unhide...", QKeySequence(), &window));
    windowMenu->addAction(makeAction("Split", QKeySequence(), &window));
    windowMenu->addAction(makeAction("Freeze Panes", QKeySequence(), &window));
    windowMenu->addSeparator();
    QAction* noWorkbooks = makeAction("(No open workbooks)", QKeySequence(), &window);
    noWorkbooks->setEnabled(false);
    windowMenu->addAction(noWorkbooks);

    auto* helpMenu = menubar->addMenu("&Help");
    helpMenu->addAction(makeAction("Microsoft Excel Help Topics", QKeySequence::HelpContents, &window));
    helpMenu->addAction(makeAction("What's This?", QKeySequence::WhatsThis, &window));
    helpMenu->addAction(makeAction("Tip of the Day", QKeySequence(), &window));
    helpMenu->addAction(makeAction("About Microsoft Excel", QKeySequence(), &window));

    auto* standardTb = new QToolBar("Standard", &window);
    standardTb->setObjectName("toolbar_standard");
    standardTb->setMovable(true);
    standardTb->addAction(aNew);
    standardTb->addAction(aOpen);
    standardTb->addAction(aSave);
    standardTb->addAction(aPrint);
    standardTb->addAction(aPrintPreview);
    standardTb->addSeparator();
    standardTb->addAction(makeAction("Spelling", QKeySequence(), &window));
    standardTb->addSeparator();
    standardTb->addAction(aCut);
    standardTb->addAction(aCopy);
    standardTb->addAction(aPaste);
    standardTb->addSeparator();
    standardTb->addAction(makeAction("Format Painter", QKeySequence(), &window));
    standardTb->addSeparator();
    standardTb->addAction(aUndo);
    standardTb->addAction(aRedo);
    standardTb->addSeparator();
    standardTb->addAction(makeAction("Chart Wizard...", QKeySequence(), &window));
    standardTb->addAction(makeAction("AutoSum", QKeySequence(), &window));
    standardTb->addSeparator();
    standardTb->addAction(makeAction("Sort Ascending", QKeySequence(), &window));
    standardTb->addAction(makeAction("Sort Descending", QKeySequence(), &window));
    standardTb->addAction(makeAction("PivotTable", QKeySequence(), &window));
    standardTb->addSeparator();
    standardTb->addAction(makeAction("Help", QKeySequence::HelpContents, &window));
    window.addToolBar(Qt::TopToolBarArea, standardTb);

    auto* formatTb = new QToolBar("Formatting", &window);
    formatTb->setObjectName("toolbar_formatting");
    formatTb->setMovable(true);

    auto* fontCombo = new QFontComboBox(formatTb);
    formatTb->addWidget(fontCombo);

    auto* sizeCombo = new QComboBox(formatTb);
    const QList<int> sizes = {8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,72};
    for (int s : sizes) sizeCombo->addItem(QString::number(s));
    sizeCombo->setCurrentText("10");
    formatTb->addWidget(sizeCombo);
    formatTb->addSeparator();

    formatTb->addAction(makeAction("Bold", QKeySequence::Bold, &window));
    formatTb->addAction(makeAction("Italic", QKeySequence::Italic, &window));
    formatTb->addAction(makeAction("Underline", QKeySequence(), &window));
    formatTb->addSeparator();
    formatTb->addAction(makeAction("Align Left", QKeySequence(), &window));
    formatTb->addAction(makeAction("Center", QKeySequence(), &window));
    formatTb->addAction(makeAction("Align Right", QKeySequence(), &window));
    formatTb->addAction(makeAction("Merge and Center", QKeySequence(), &window));
    formatTb->addSeparator();
    formatTb->addAction(makeAction("Currency Style", QKeySequence(), &window));
    formatTb->addAction(makeAction("Percent Style", QKeySequence(), &window));
    formatTb->addAction(makeAction("Comma Style", QKeySequence(), &window));
    formatTb->addSeparator();
    formatTb->addAction(makeAction("Increase Decimal", QKeySequence(), &window));
    formatTb->addAction(makeAction("Decrease Decimal", QKeySequence(), &window));
    formatTb->addSeparator();
    formatTb->addAction(makeAction("Increase Indent", QKeySequence(), &window));
    formatTb->addAction(makeAction("Decrease Indent", QKeySequence(), &window));
    formatTb->addSeparator();
    formatTb->addAction(makeAction("Borders", QKeySequence(), &window));
    formatTb->addAction(makeAction("Pattern", QKeySequence(), &window));
    formatTb->addAction(makeAction("Font Color", QKeySequence(), &window));
    window.addToolBar(Qt::TopToolBarArea, formatTb);

    auto showMsg = [&](const QString& m){ window.statusBar()->showMessage(m, 3000); };

    QObject::connect(aNew, &QAction::triggered, [&]{ showMsg("New workbook"); });
    QObject::connect(aOpen, &QAction::triggered, [&]{ showMsg("Open workbook"); });
    QObject::connect(aSave, &QAction::triggered, [&]{ showMsg("Save workbook"); });
    QObject::connect(aPrint, &QAction::triggered, [&]{ showMsg("Print workbook"); });
    QObject::connect(aPrintPreview, &QAction::triggered, [&]{ showMsg("Print preview"); });
    QObject::connect(aExit, &QAction::triggered, &app, &QApplication::quit);
    QObject::connect(aCut, &QAction::triggered, [&]{ showMsg("Cut"); });
    QObject::connect(aCopy, &QAction::triggered, [&]{ showMsg("Copy"); });
    QObject::connect(aPaste, &QAction::triggered, [&]{ showMsg("Paste"); });
    QObject::connect(aUndo, &QAction::triggered, [&]{ showMsg("Undo"); });
    QObject::connect(aRedo, &QAction::triggered, [&]{ showMsg("Redo"); });

    auto* toolbarsAction = makeAction("Toggle Toolbars", QKeySequence(), &window);
    QObject::connect(toolbarsAction, &QAction::triggered, [&]{
        formatTb->setVisible(!formatTb->isVisible());
        showMsg(formatTb->isVisible() ? "Formatting toolbar visible" : "Formatting toolbar hidden");
    });
    viewMenu->addAction(toolbarsAction);

    QObject::connect(fontCombo, &QFontComboBox::currentFontChanged, [&](const QFont& f){
        showMsg(QString("Font: %1").arg(f.family()));
    });
    QObject::connect(sizeCombo, &QComboBox::currentTextChanged, [&](const QString& s){
        showMsg(QString("Size: %1").arg(s));
    });

    window.show();
    return app.exec();
}
