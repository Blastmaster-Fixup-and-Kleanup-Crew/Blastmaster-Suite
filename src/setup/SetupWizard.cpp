#include "SetupWizard.h"

#include "blastmaster/ProductKeyValidator.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWizardPage>

namespace
{
constexpr const char* STANDARD_KEY =
    "CHKZMF0MKMPWRKIRVYXIV9SV";

constexpr const char* PROFESSIONAL_KEY =
    "JLGCDIQR9ZTNXR79BFUB9OELD";

QString normalizeKey(const QString& key)
{
    QString result;

    for (const QChar& ch : key)
    {
        if (ch.isLetterOrNumber())
            result += ch.toUpper();
    }

    return result;
}

bool isStandardKey(const QString& key)
{
    return normalizeKey(key) == QString::fromLatin1(STANDARD_KEY);
}

bool isProfessionalKey(const QString& key)
{
    return normalizeKey(key) == QString::fromLatin1(PROFESSIONAL_KEY);
}

QString detectedEdition(const QString& key)
{
    if (isStandardKey(key))
        return QStringLiteral("Standard");

    if (isProfessionalKey(key))
        return QStringLiteral("Professional");

    return QString();
}
}

SetupWizard::SetupWizard(QWidget* parent)
    : QWizard(parent)
{
    setupAppearance();
    createPages();

    setWindowTitle(QStringLiteral("Blastmaster Suite Setup"));
    setFixedSize(520, 360);
}

void SetupWizard::setupAppearance()
{
    QFont systemFont(QStringLiteral("MS Sans Serif"));
    systemFont.setPointSize(8);

    QApplication::setFont(systemFont);

    setWizardStyle(QWizard::ClassicStyle);

    setStyleSheet(R"(
        QWizard {
            background-color: #c0c0c0;
            color: #000000;
        }

        QWizardPage {
            background-color: #c0c0c0;
            color: #000000;
        }

        QLabel {
            background-color: transparent;
            color: #000000;
            font-family: "MS Sans Serif";
            font-size: 8pt;
        }

        QLineEdit {
            background-color: #ffffff;
            color: #000000;
            border-top: 2px solid #808080;
            border-left: 2px solid #808080;
            border-right: 2px solid #ffffff;
            border-bottom: 2px solid #ffffff;
            padding: 2px;
            font-family: "MS Sans Serif";
            font-size: 8pt;
        }

        QTextEdit {
            background-color: #ffffff;
            color: #000000;
            border-top: 2px solid #808080;
            border-left: 2px solid #808080;
            border-right: 2px solid #ffffff;
            border-bottom: 2px solid #ffffff;
            font-family: "MS Sans Serif";
            font-size: 8pt;
        }

        QPushButton {
            background-color: #c0c0c0;
            color: #000000;
            border-top: 2px solid #ffffff;
            border-left: 2px solid #ffffff;
            border-right: 2px solid #404040;
            border-bottom: 2px solid #404040;
            padding: 3px 12px;
            min-width: 60px;
            min-height: 18px;
            font-family: "MS Sans Serif";
            font-size: 8pt;
        }

        QPushButton:pressed {
            border-top: 2px solid #404040;
            border-left: 2px solid #404040;
            border-right: 2px solid #ffffff;
            border-bottom: 2px solid #ffffff;
        }

        QComboBox {
            background-color: #ffffff;
            color: #000000;
            border-top: 2px solid #808080;
            border-left: 2px solid #808080;
            border-right: 2px solid #ffffff;
            border-bottom: 2px solid #ffffff;
            padding: 2px;
        }

        QProgressBar {
            background-color: #ffffff;
            color: #000000;
            border-top: 2px solid #808080;
            border-left: 2px solid #808080;
            border-right: 2px solid #ffffff;
            border-bottom: 2px solid #ffffff;
            text-align: center;
        }

        QProgressBar::chunk {
            background-color: #000080;
        }
    )");
}

void SetupWizard::createPages()
{
    addPage(createWelcomePage());
    addPage(createLicensePage());
    addPage(createProductKeyPage());
    addPage(createDestinationPage());
    addPage(createReadyPage());
    addPage(createInstallPage());
    addPage(createFinishedPage());
}

QWizardPage* SetupWizard::createWelcomePage()
{
    auto* page = new QWizardPage;
    page->setTitle(QStringLiteral("Welcome to Blastmaster Suite Setup"));

    auto* layout = new QVBoxLayout(page);

    auto* title = new QLabel(
        QStringLiteral("<b>Welcome to Blastmaster Suite Setup</b>"));
    title->setStyleSheet(
        "font-family: 'MS Sans Serif'; font-size: 10pt;");

    auto* text = new QLabel(
        QStringLiteral(
            "This wizard will install Blastmaster Suite on your computer.\n\n"
            "Blastmaster Suite includes applications for documents, "
            "spreadsheets, presentations, and other office tasks.\n\n"
            "Click Next to continue."));
    text->setWordWrap(true);

    layout->addSpacing(15);
    layout->addWidget(title);
    layout->addSpacing(12);
    layout->addWidget(text);
    layout->addStretch();

    return page;
}

QWizardPage* SetupWizard::createLicensePage()
{
    auto* page = new QWizardPage;
    page->setTitle(QStringLiteral("License Agreement"));

    auto* layout = new QVBoxLayout(page);

    auto* license = new QTextEdit;
    license->setReadOnly(true);

    license->setPlainText(
        "BLASTMASTER SUITE LICENSE AGREEMENT\n"
        "\n"
        "Please read the following license agreement before "
        "continuing with the installation.\n"
        "\n"
        "Blastmaster Suite is provided for use in accordance with "
        "the terms supplied with this software.\n"
        "\n"
        "By installing this software, you agree to comply with "
        "the applicable license terms.\n");

    layout->addWidget(license);

    return page;
}

QWizardPage* SetupWizard::createProductKeyPage()
{
    auto* page = new QWizardPage;

    page->setTitle(QStringLiteral("Product Key"));
    page->setSubTitle(
        QStringLiteral(
            "Enter your Blastmaster Suite product key."));

    auto* layout = new QVBoxLayout(page);

    auto* instruction = new QLabel(
        QStringLiteral(
            "Type the 25-character product key supplied with "
            "your copy of Blastmaster Suite."));
    instruction->setWordWrap(true);

    productKeyEdit_ = new QLineEdit;
    productKeyEdit_->setPlaceholderText(
        QStringLiteral("XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"));
    productKeyEdit_->setMaxLength(29);

    keyStatusLabel_ = new QLabel;
    keyStatusLabel_->setText(
        QStringLiteral(""));

    layout->addWidget(instruction);
    layout->addSpacing(12);
    layout->addWidget(productKeyEdit_);
    layout->addSpacing(8);
    layout->addWidget(keyStatusLabel_);
    layout->addStretch();

    connect(productKeyEdit_, &QLineEdit::textChanged,
            this, [this]()
    {
        const QString edition = detectedEdition(productKeyEdit_->text());

        if (edition.isEmpty())
        {
            keyStatusLabel_->setText(
                QStringLiteral("Enter a valid Blastmaster Suite product key."));
        }
        else
        {
            keyStatusLabel_->setText(
                QStringLiteral("Product key recognized: %1 edition.")
                    .arg(edition));
        }

        emit completeChanged();
    });

    registerField(
        QStringLiteral("productKey*"),
        productKeyEdit_);

    return page;
}

QWizardPage* SetupWizard::createDestinationPage()
{
    auto* page = new QWizardPage;

    page->setTitle(QStringLiteral("Choose Destination Location"));
    page->setSubTitle(
        QStringLiteral(
            "Choose the folder where Blastmaster Suite will be installed."));

    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel(
        QStringLiteral("Destination folder:"));

    auto* row = new QHBoxLayout;

    destinationEdit_ = new QLineEdit;
    destinationEdit_->setText(
        QStringLiteral("C:/Program Files/Blastmaster Suite"));

    auto* browseButton = new QPushButton(
        QStringLiteral("Browse..."));

    row->addWidget(destinationEdit_);
    row->addWidget(browseButton);

    layout->addWidget(label);
    layout->addSpacing(8);
    layout->addLayout(row);
    layout->addStretch();

    connect(browseButton, &QPushButton::clicked,
            this, [this]()
    {
        const QString directory =
            QFileDialog::getExistingDirectory(
                this,
                QStringLiteral("Select Destination Folder"),
                destinationEdit_->text());

        if (!directory.isEmpty())
            destinationEdit_->setText(directory);
    });

    return page;
}

QWizardPage* SetupWizard::createReadyPage()
{
    auto* page = new QWizardPage;

    page->setTitle(QStringLiteral("Ready to Install"));

    auto* layout = new QVBoxLayout(page);

    readyLabel_ = new QLabel;
    readyLabel_->setWordWrap(true);

    layout->addSpacing(15);
    layout->addWidget(readyLabel_);
    layout->addStretch();

    connect(page, &QWizardPage::completeChanged,
            this, [this]()
    {
        if (!readyLabel_)
            return;

        const QString edition =
            detectedEdition(productKeyEdit_->text());

        readyLabel_->setText(
            QStringLiteral(
                "Setup is ready to install Blastmaster Suite.\n\n"
                "Edition: %1\n"
                "Destination:\n"
                "%2\n\n"
                "Click Install to begin the installation.")
                .arg(edition.isEmpty()
                         ? QStringLiteral("Unknown")
                         : edition)
                .arg(destinationEdit_->text()));
    });

    return page;
}

QWizardPage* SetupWizard::createInstallPage()
{
    auto* page = new QWizardPage;

    page->setTitle(QStringLiteral("Installing Blastmaster Suite"));

    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel(
        QStringLiteral(
            "Please wait while Blastmaster Suite is installed."));
    label->setWordWrap(true);

    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);

    layout->addSpacing(20);
    layout->addWidget(label);
    layout->addSpacing(15);
    layout->addWidget(progressBar_);
    layout->addStretch();

    connect(page, &QWizardPage::initializePage,
            this, [this]()
    {
        progressBar_->setValue(0);

        QTimer* timer = new QTimer(this);

        connect(timer, &QTimer::timeout,
                this, [this, timer]()
        {
            int value = progressBar_->value();

            if (value >= 100)
            {
                timer->stop();
                timer->deleteLater();
                return;
            }

            progressBar_->setValue(value + 5);
        });

        timer->start(100);
    });

    return page;
}

QWizardPage* SetupWizard::createFinishedPage()
{
    auto* page = new QWizardPage;

    page->setTitle(QStringLiteral("Installation Complete"));

    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel(
        QStringLiteral(
            "Blastmaster Suite has been successfully installed.\n\n"
            "Click Finish to close Setup."));
    label->setWordWrap(true);

    layout->addSpacing(25);
    layout->addWidget(label);
    layout->addStretch();

    return page;
}

bool SetupWizard::validateProductKey()
{
    const QString key = productKeyEdit_->text();

    if (isStandardKey(key) || isProfessionalKey(key))
        return true;

    QMessageBox::warning(
        this,
        QStringLiteral("Invalid Product Key"),
        QStringLiteral(
            "The product key you entered is not recognized.\n\n"
            "Please check the key and try again."));

    return false;
}
