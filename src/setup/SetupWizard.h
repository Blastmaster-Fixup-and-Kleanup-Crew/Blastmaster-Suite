#pragma once

#include <QWizard>
#include <QString>

class QLineEdit;
class QLabel;
class QComboBox;
class QProgressBar;

class SetupWizard final : public QWizard
{
    Q_OBJECT

public:
    explicit SetupWizard(QWidget* parent = nullptr);

private:
    void setupAppearance();
    void createPages();

    QWizardPage* createWelcomePage();
    QWizardPage* createLicensePage();
    QWizardPage* createProductKeyPage();
    QWizardPage* createDestinationPage();
    QWizardPage* createReadyPage();
    QWizardPage* createInstallPage();
    QWizardPage* createFinishedPage();

    bool validateProductKey();

    QLineEdit* productKeyEdit_ = nullptr;
    QLabel* keyStatusLabel_ = nullptr;

    QLineEdit* destinationEdit_ = nullptr;

    QLabel* editionLabel_ = nullptr;
    QLabel* readyLabel_ = nullptr;

    QProgressBar* progressBar_ = nullptr;
};
