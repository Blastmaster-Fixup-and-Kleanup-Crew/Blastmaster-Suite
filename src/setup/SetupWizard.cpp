#include "SetupWizard.h"

#include "InstallerBackend.h"
#include "blastmaster/ProductKeyValidator.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWizardPage>

namespace
{
std::string toStdString(const QString& value)
{
    return value.toStdString();
}
}

SetupWizard::SetupWizard(QWidget* parent)
    : QWizard(parent)
{
    setupAppearance();
    createPages();

    setWindowTitle(
        QStringLiteral("Blastmaster Suite Setup"));

    setFixedSize(520, 360);
}

void SetupWizard::setupAppearance()
{
    QFont systemFont(
        QStringLiteral("MS Sans Serif"));

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

    page->setTitle(
        QStringLiteral(
            "Welcome to Blastmaster Suite Setup"));

    auto* layout = new QVBoxLayout(page);

    auto* title = new QLabel(
        QStringLiteral(
            "<b>Welcome to Blastmaster Suite Setup</b>"));

    title->setStyleSheet(
        "font-family: 'MS Sans Serif'; "
        "font-size: 10pt;");

    auto* text = new QLabel(
        QStringLiteral(
            "This wizard will install Blastmaster Suite "
            "on your computer.\n\n"
            "Blastmaster Suite includes applications for "
            "documents, spreadsheets, presentations, and "
            "other office tasks.\n\n"
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

    page->setTitle(
        QStringLiteral("License Agreement"));

    auto* layout = new QVBoxLayout(page);

    auto* license = new QTextEdit;

    license->setReadOnly(true);

    license->setPlainText(
        "BLASTMASTER SUITE LICENSE AGREEMENT\n"
        "\n"
        "Please read the following license agreement "
        "before continuing with the installation.\n"
        "\n"
        "Blastmaster Suite is provided for use in "
        "accordance with the terms supplied with this "
        "software.\n"
        "\n"
        "By installing this software, you agree to "
        "comply with the applicable license terms.\n");

    layout->addWidget(license);

    return page;
}

QWizardPage* SetupWizard::createProductKeyPage()
{
    auto* page = new QWizardPage;

    page->setTitle(
        QStringLiteral("Product Key"));

    page->setSubTitle(
        QStringLiteral(
            "Enter your Blastmaster Suite product key."));

    auto* layout = new QVBoxLayout(page);

    auto* instruction = new QLabel(
        QStringLiteral(
            "Type the 25-character product key supplied "
            "with your copy of Blastmaster Suite."));

    instruction->setWordWrap(true);

    productKeyEdit_ = new QLineEdit;

    productKeyEdit_->setPlaceholderText(
        QStringLiteral(
            "XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"));

    productKeyEdit_->setMaxLength(29);

    keyStatusLabel_ = new QLabel;

    keyStatusLabel_->setWordWrap(true);

    layout->addWidget(instruction);
    layout->addSpacing(12);
    layout->addWidget(productKeyEdit_);
    layout->addSpacing(8);
    layout->addWidget(keyStatusLabel_);
    layout->addStretch();

    registerField(
        QStringLiteral("productKey*"),
        productKeyEdit_);

    connect(
        productKeyEdit_,
        &QLineEdit::textChanged,
        this,
        [this]()
        {
            updateProductKeyStatus();

            emit completeChanged();
        });

    updateProductKeyStatus();

    return page;
}

QWizardPage* SetupWizard::createDestinationPage()
{
    auto* page = new QWizardPage;

    page->setTitle(
        QStringLiteral(
            "Choose Destination Location"));

    page->setSubTitle(
        QStringLiteral(
            "Choose the folder where Blastmaster Suite "
            "will be installed."));

    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel(
        QStringLiteral("Destination folder:"));

    auto* row = new QHBoxLayout;

    destinationEdit_ = new QLineEdit;

    destinationEdit_->setText(
        QStringLiteral(
            "C:/Program Files/Blastmaster Suite"));

    auto* browseButton = new QPushButton(
        QStringLiteral("Browse..."));

    row->addWidget(destinationEdit_);
    row->addWidget(browseButton);

    layout->addWidget(label);
    layout->addSpacing(8);
    layout->addLayout(row);
    layout->addStretch();

    connect(
        browseButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const QString directory =
                QFileDialog::getExistingDirectory(
                    this,
                    QStringLiteral(
                        "Select Destination Folder"),
                    destinationEdit_->text());

            if (!directory.isEmpty())
                destinationEdit_->setText(directory);
        });

    return page;
}

QWizardPage* SetupWizard::createReadyPage()
{
    auto* page = new QWizardPage;

    page->setTitle(
        QStringLiteral("Ready to Install"));

    auto* layout = new QVBoxLayout(page);

    readyLabel_ = new QLabel;

    readyLabel_->setWordWrap(true);

    layout->addSpacing(15);
    layout->addWidget(readyLabel_);
    layout->addStretch();

    connect(
        page,
        &QWizardPage::initializePage,
        this,
        [this]()
        {
            readyLabel_->setText(
                QStringLiteral(
                    "Setup is ready to install Blastmaster Suite.\\n\\n"
                    "Edition: %1\\n\\n"
                    "Destination:\\n%2\\n\\n"
                    "Click Install to begin the installation.")
                    .arg(editionName())
                    .arg(destinationEdit_->text()));
        });

    return page;
}

QWizardPage* SetupWizard::createInstallPage()
{
    auto* page = new QWizardPage;

    page->setTitle(
        QStringLiteral(
            "Installing Blastmaster Suite"));

    auto* layout = new QVBoxLayout(page);
    auto* label = new QLabel;
    label->setWordWrap(true);

    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);

    layout->addSpacing(20);
    layout->addWidget(label);
    layout->addSpacing(15);
    layout->addWidget(progressBar_);
    layout->addStretch();

    connect(
        page,
        &QWizardPage::initializePage,
        this,
        [this, label]()
        {
            const QString edition = editionName();

            label->setText(
                QStringLiteral(
                    "Installing Blastmaster Suite "
                    "%1 edition.\\n\\n"
                    "Please wait while the software is installed.")
                    .arg(edition));

            progressBar_->setValue(10);
            QApplication::processEvents();

            InstallerBackend installer(
                QDir(QCoreApplication::applicationDirPath())
                    .filePath(QStringLiteral("payload")));

            const InstallerBackend::Result result =
                installer.install(
                    detectedEdition_,
                    destinationEdit_->text(),
                    productKeyEdit_->text());

            installSucceeded_ = result.success;

            if (!result.success)
            {
                installedLocation_.clear();
                progressBar_->setValue(0);

                QMessageBox::critical(
                    this,
                    QStringLiteral("Installation Failed"),
                    result.message);

                label->setText(
                    QStringLiteral(
                        "Installation failed.\\n\\n"
                        "Please go back and correct the "
                        "installation settings, then try again."));
                return;
            }

            progressBar_->setValue(100);
            installedLocation_ = result.installedLocation;

            label->setText(
                QStringLiteral(
                    "Blastmaster Suite %1 edition was installed "
                    "successfully.\\n\\n"
                    "Installed to:\\n%2")
                    .arg(edition)
                    .arg(result.installedLocation));
        });

    return page;
}

QWizardPage* SetupWizard::createFinishedPage()
{
    auto* page = new QWizardPage;

    page->setTitle(
        QStringLiteral(
            "Installation Complete"));

    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel;
    label->setWordWrap(true);

    layout->addSpacing(25);
    layout->addWidget(label);
    layout->addStretch();

    connect(
        page,
        &QWizardPage::initializePage,
        this,
        [this, label]()
        {
            label->setText(
                QStringLiteral(
                    "Blastmaster Suite %1 edition has been "
                    "installed successfully.\\n\\n"
                    "Installation folder:\\n%2\\n\\n"
                    "Click Finish to close Setup.")
                    .arg(editionName())
                    .arg(installedLocation_));
        });

    return page;
}

bool SetupWizard::validateProductKey()
{
    if (!productKeyEdit_)
        return false;

    const QString key =
        productKeyEdit_->text();

    detectedEdition_ =
        blastmaster::ProductKeyValidator::detect_edition(
            toStdString(key));

    if (detectedEdition_ ==
        blastmaster::Edition::Invalid)
    {
        QMessageBox::warning(
            this,
            QStringLiteral("Invalid Product Key"),
            QStringLiteral(
                "The product key you entered is not "
                "recognized.\n\n"
                "Please check the key and try again."));

        return false;
    }

    return true;
}

void SetupWizard::updateProductKeyStatus()
{
    if (!productKeyEdit_ || !keyStatusLabel_)
        return;

    const QString key =
        productKeyEdit_->text();

    detectedEdition_ =
        blastmaster::ProductKeyValidator::detect_edition(
            toStdString(key));

    switch (detectedEdition_)
    {
    case blastmaster::Edition::Standard:
        keyStatusLabel_->setText(
            QStringLiteral(
                "Product key recognized: "
                "Standard edition."));
        break;

    case blastmaster::Edition::Professional:
        keyStatusLabel_->setText(
            QStringLiteral(
                "Product key recognized: "
                "Professional edition."));
        break;

    case blastmaster::Edition::Invalid:
    default:
        keyStatusLabel_->setText(
            key.isEmpty()
                ? QString()
                : QStringLiteral(
                    "The product key is not valid."));
        break;
    }
}

QString SetupWizard::editionName() const
{
    switch (detectedEdition_)
    {
    case blastmaster::Edition::Standard:
        return QStringLiteral("Standard");

    case blastmaster::Edition::Professional:
        return QStringLiteral("Professional");

    case blastmaster::Edition::Invalid:
    default:
        return QStringLiteral("Unknown");
    }
}

bool SetupWizard::validateCurrentPage()
{
    /*
     * Product Key is the third page.
     *
     * Qt page IDs start at 0:
     *   0 = Welcome
     *   1 = License
     *   2 = Product Key
     */
    if (currentId() == 2)
    {
        return validateProductKey();
    }

    if (currentId() == 3)
    {
        const QString destination = destinationEdit_
            ? destinationEdit_->text().trimmed()
            : QString();

        if (destination.isEmpty())
        {
            QMessageBox::warning(
                this,
                QStringLiteral("Installation Folder"),
                QStringLiteral("Please choose an installation folder."));
            return false;
        }

        return true;
    }

    if (currentId() == 5)
        return installSucceeded_;

    return QWizard::validateCurrentPage();
}
