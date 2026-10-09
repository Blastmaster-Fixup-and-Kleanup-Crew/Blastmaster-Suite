#include "SetupWizard.h"

#include "InstallerBackend.h"
#include "blastmaster/ProductKeyValidator.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWizardPage>

namespace
{
std::string toStdString(const QString& value) { return value.toStdString(); }

QString normalizedKey(const QString& value)
{
    QString result;
    for (const QChar ch : value)
        if (ch.isLetterOrNumber())
            result.append(ch.toUpper());
    return result.left(25);
}
}

SetupWizard::SetupWizard(QWidget* parent)
    : QWizard(parent)
{
    setWindowTitle(tr("Blastmaster Suite Setup"));
    setWizardStyle(QWizard::ClassicStyle);
    setButtonText(QWizard::BackButton, tr("< Back"));
    setButtonText(QWizard::NextButton, tr("Next >"));
    setButtonText(QWizard::CancelButton, tr("Cancel"));
    setButtonText(QWizard::FinishButton, tr("Finish"));
    setFixedSize(560, 380);

    const QString brandingIcon = qEnvironmentVariable("BLASTMASTER_SETUP_BRANDING").isEmpty()
        ? QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("blastmaster_suite_setup.svg"))
        : qEnvironmentVariable("BLASTMASTER_SETUP_BRANDING");
    if (QFile::exists(brandingIcon))
        setWindowIcon(QIcon(brandingIcon));

    setupAppearance();
    createPages();
}

void SetupWizard::setupAppearance()
{
    QFont font(QStringLiteral("MS Sans Serif"));
    font.setPointSize(8);
    QApplication::setFont(font);

    setStyleSheet(R"(
        QWizard, QWizardPage { background: #c0c0c0; color: #000000; }
        QLabel { background: transparent; color: #000000; font-family: "MS Sans Serif"; font-size: 8pt; }
        QLineEdit { background: #ffffff; color: #000000; border-top: 2px solid #808080; border-left: 2px solid #808080;
                    border-right: 2px solid #ffffff; border-bottom: 2px solid #ffffff; padding: 2px; }
        QPushButton { background: #c0c0c0; color: #000000; border-top: 2px solid #ffffff; border-left: 2px solid #ffffff;
                      border-right: 2px solid #404040; border-bottom: 2px solid #404040; padding: 3px 12px;
                      min-width: 60px; min-height: 18px; }
        QPushButton:pressed { border-top: 2px solid #404040; border-left: 2px solid #404040;
                               border-right: 2px solid #ffffff; border-bottom: 2px solid #ffffff; }
        QProgressBar { background: #ffffff; color: #000000; border-top: 2px solid #808080; border-left: 2px solid #808080;
                       border-right: 2px solid #ffffff; border-bottom: 2px solid #ffffff; text-align: center; }
        QProgressBar::chunk { background: #000080; }
    )");
}

void SetupWizard::createPages()
{
    addPage(createWelcomePage());
    addPage(createProductKeyPage());
    addPage(createDestinationPage());
    addPage(createReadyPage());
    addPage(createInstallPage());
    addPage(createFinishedPage());
}

QWizardPage* SetupWizard::createWelcomePage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Welcome to Blastmaster Suite Setup"));
    auto* layout = new QVBoxLayout(page);
    auto* branding = new QLabel;
    const QString brandingPath = qEnvironmentVariable("BLASTMASTER_SETUP_BRANDING");
    if (!brandingPath.isEmpty() && QFile::exists(brandingPath))
    {
        const QIcon icon(brandingPath);
        branding->setPixmap(icon.pixmap(QSize(48, 48)));
        branding->setFixedSize(48, 48);
    }
    auto* title = new QLabel(tr("<b>Blastmaster Suite</b>"));
    title->setStyleSheet("font-size: 12pt;");
    auto* text = new QLabel(
        tr("Welcome to Blastmaster Suite Setup.\n\n"
                       "This wizard installs or upgrades Blastmaster Suite on your computer.\n\n"
                       "Click Next to continue."));
    text->setWordWrap(true);
    layout->addSpacing(12);
    layout->addWidget(branding);
    layout->addWidget(title);
    layout->addSpacing(12);
    layout->addWidget(text);
    layout->addStretch();
    return page;
}

QWizardPage* SetupWizard::createProductKeyPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Product Key"));
    page->setSubTitle(tr("Enter your 25-character Blastmaster Suite product key."));

    auto* layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel(
        tr("The key is formatted automatically as you type or paste it.")));

    productKeyEdit_ = new QLineEdit;
    productKeyEdit_->setAccessibleName(tr("Product key"));
    productKeyEdit_->setAccessibleDescription(tr("Enter the 25-character Blastmaster Suite product key."));
    productKeyEdit_->setPlaceholderText(tr("XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"));
    productKeyEdit_->setMaxLength(29);
    productKeyEdit_->setClearButtonEnabled(true);

    keyStatusLabel_ = new QLabel;
    keyStatusLabel_->setWordWrap(true);

    layout->addSpacing(10);
    layout->addWidget(productKeyEdit_);
    layout->addSpacing(8);
    layout->addWidget(keyStatusLabel_);
    layout->addStretch();

    connect(productKeyEdit_, &QLineEdit::textChanged, this, [this]()
    {
        formatProductKey();
        updateProductKeyStatus();
    });
    return page;
}

void SetupWizard::formatProductKey()
{
    if (!productKeyEdit_)
        return;

    const QString normalized = normalizedKey(productKeyEdit_->text());
    QString formatted;
    for (int i = 0; i < normalized.size(); ++i)
    {
        if (i > 0 && i % 5 == 0)
            formatted.append('-');
        formatted.append(normalized.at(i));
    }

    if (productKeyEdit_->text() == formatted)
        return;

    const int oldPosition = productKeyEdit_->cursorPosition();
    productKeyEdit_->blockSignals(true);
    productKeyEdit_->setText(formatted);
    productKeyEdit_->setCursorPosition(qMin(formatted.size(), oldPosition));
    productKeyEdit_->blockSignals(false);
}

QWizardPage* SetupWizard::createDestinationPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Choose Destination Location"));
    page->setSubTitle(tr("Choose where Blastmaster Suite will be installed."));

    auto* layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel(tr("Destination folder:")));

    auto* row = new QHBoxLayout;
    QString programFiles = qEnvironmentVariable("ProgramFiles");
    if (programFiles.isEmpty())
        programFiles = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    destinationEdit_ = new QLineEdit(
        QDir::fromNativeSeparators(
            QDir(programFiles).filePath(QStringLiteral("Blastmaster Suite"))));
    destinationEdit_->setAccessibleName(tr("Installation destination"));
    destinationEdit_->setAccessibleDescription(tr("Folder where Blastmaster Suite will be installed."));
    auto* browse = new QPushButton(tr("&Browse..."));
    browse->setAccessibleName(tr("Browse for installation folder"));
    browse->setAccessibleDescription(tr("Choose the installation destination folder."));
    row->addWidget(destinationEdit_);
    row->addWidget(browse);
    layout->addLayout(row);

    destinationStatusLabel_ = new QLabel;
    destinationStatusLabel_->setWordWrap(true);
    layout->addWidget(destinationStatusLabel_);
    layout->addStretch();

    connect(browse, &QPushButton::clicked, this, [this]()
    {
        const QString selected = QFileDialog::getExistingDirectory(
            this, tr("Select Destination Folder"), destinationEdit_->text());
        if (!selected.isEmpty())
            destinationEdit_->setText(selected);
        validateDestination();
    });

    connect(destinationEdit_, &QLineEdit::textChanged, this, [this]()
    {
        validateDestination();
    });

    validateDestination();
    return page;
}

bool SetupWizard::validateDestination()
{
    if (!destinationEdit_ || !destinationStatusLabel_)
        return false;

    const QString path = destinationEdit_->text().trimmed();
    if (path.isEmpty())
    {
        destinationStatusLabel_->setText(tr("Choose an installation folder."));
        return false;
    }

    QFileInfo info(path);
    if (info.exists() && !info.isDir())
    {
        destinationStatusLabel_->setText(tr("The destination exists but is not a folder."));
        return false;
    }

    if (path.size() > 240)
    {
        destinationStatusLabel_->setText(tr("The installation path is too long."));
        return false;
    }

    const QString parentPath = info.exists() ? path : info.absolutePath();
    QDir parent(parentPath);
    if (!parent.exists() && !QDir().mkpath(parentPath))
    {
        destinationStatusLabel_->setText(tr("Setup cannot create this folder."));
        return false;
    }

    const QString testFile = QDir(parentPath).filePath(QStringLiteral(".blastmaster_write_test"));
    QFile test(testFile);
    if (!test.open(QIODevice::WriteOnly))
    {
        destinationStatusLabel_->setText(
            tr("Setup cannot write to this location. Choose a folder with write access."));
        return false;
    }
    test.close();
    QFile::remove(testFile);

    const QString activation =
        QDir(path).filePath(QStringLiteral("config/activation.ini"));
    if (QFile::exists(activation))
    {
        QSettings settings(activation, QSettings::IniFormat);
        const QString installedEdition =
            settings.value(QStringLiteral("Blastmaster Suite/Edition")).toString();

        if (!installedEdition.isEmpty())
        {
            destinationStatusLabel_->setText(
                tr("Existing %1 installation detected. Continuing will upgrade/reinstall it.")
                    .arg(installedEdition));
            return true;
        }
    }

    destinationStatusLabel_->setText(tr("Installation location is available."));
    return true;
}

QWizardPage* SetupWizard::createReadyPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Ready to Install"));
    page->setCommitPage(true);

    auto* layout = new QVBoxLayout(page);
    readyLabel_ = new QLabel;
    readyLabel_->setWordWrap(true);
    layout->addSpacing(12);
    layout->addWidget(readyLabel_);
    layout->addStretch();

    connect(page, &QWizardPage::initializePage, this, [this]()
    {
        const QString activation =
            QDir(destinationEdit_->text()).filePath(QStringLiteral("config/activation.ini"));
        const bool upgrade = QFile::exists(activation);

        readyLabel_->setText(
            tr("Setup is ready to %1 Blastmaster Suite.\n\n"
                           "Edition: %2\n\nDestination:\n%3\n\n"
                           "Click Install to begin.")
                .arg(upgrade ? tr("upgrade/reinstall")
                             : tr("install"))
                .arg(editionName())
                .arg(destinationEdit_->text()));
    });
    return page;
}

QWizardPage* SetupWizard::createInstallPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Installing Blastmaster Suite"));

    auto* layout = new QVBoxLayout(page);
    auto* label = new QLabel;
    label->setWordWrap(true);
    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 100);
    layout->addSpacing(15);
    layout->addWidget(label);
    layout->addSpacing(12);
    layout->addWidget(progressBar_);
    layout->addStretch();

    connect(page, &QWizardPage::initializePage, this, [this, label]()
    {
        installSucceeded_ = false;
        progressBar_->setValue(10);
        label->setText(tr("Installing Blastmaster Suite..."));
        QApplication::processEvents();

        QString payloadRoot = qEnvironmentVariable("BLASTMASTER_SETUP_PAYLOAD");
        if (payloadRoot.isEmpty())
            payloadRoot = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("payload"));

        InstallerBackend installer(payloadRoot);
        const auto result = installer.install(
            detectedEdition_, destinationEdit_->text(), productKeyEdit_->text());

        if (!result.success)
        {
            progressBar_->setValue(0);
            label->setText(
                tr("Setup could not complete the installation.\n\n%1\n\n"
                               "Click Back to change the settings and try again.")
                    .arg(result.message));
            QMessageBox::critical(this, tr("Installation Failed"), result.message);
            return;
        }

        installSucceeded_ = true;
        progressBar_->setValue(100);
        installedLocation_ = result.installedLocation;
        label->setText(tr("Blastmaster Suite was installed successfully.\n\n%1")
                           .arg(installedLocation_));
    });
    return page;
}

QWizardPage* SetupWizard::createFinishedPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Installation Complete"));

    auto* layout = new QVBoxLayout(page);
    auto* label = new QLabel;
    label->setWordWrap(true);
    layout->addSpacing(20);
    layout->addWidget(label);
    layout->addStretch();

    connect(page, &QWizardPage::initializePage, this, [this, label]()
    {
        label->setText(
            tr("Blastmaster Suite %1 edition is ready to use.\n\n"
                           "Installed to:\n%2\n\nClick Finish to close Setup.")
                .arg(editionName(), installedLocation_));
    });
    return page;
}

bool SetupWizard::validateProductKey()
{
    detectedEdition_ =
        blastmaster::ProductKeyValidator::detect_edition(toStdString(productKeyEdit_->text()));

    if (detectedEdition_ == blastmaster::Edition::Invalid)
    {
        QMessageBox::warning(
            this, tr("Invalid Product Key"),
            tr("That product key is not recognized.\n\n"
                           "Enter the key as XXXXX-XXXXX-XXXXX-XXXXX-XXXXX."));
        return false;
    }
    return true;
}

void SetupWizard::updateProductKeyStatus()
{
    if (!productKeyEdit_ || !keyStatusLabel_)
        return;

    detectedEdition_ =
        blastmaster::ProductKeyValidator::detect_edition(toStdString(productKeyEdit_->text()));

    if (detectedEdition_ == blastmaster::Edition::Standard)
        keyStatusLabel_->setText(tr("Product key recognized — Standard edition."));
    else if (detectedEdition_ == blastmaster::Edition::Professional)
        keyStatusLabel_->setText(tr("Product key recognized — Professional edition."));
    else if (!productKeyEdit_->text().isEmpty())
        keyStatusLabel_->setText(tr("Product key not recognized yet."));
    else
        keyStatusLabel_->clear();
}

QString SetupWizard::editionName() const
{
    switch (detectedEdition_)
    {
    case blastmaster::Edition::Standard: return tr("Standard");
    case blastmaster::Edition::Professional: return tr("Professional");
    default: return tr("Unknown");
    }
}

bool SetupWizard::validateCurrentPage()
{
    if (currentId() == 1)
        return validateProductKey();
    if (currentId() == 2)
        return validateDestination();
    if (currentId() == 4)
        return installSucceeded_;
    return QWizard::validateCurrentPage();
}
