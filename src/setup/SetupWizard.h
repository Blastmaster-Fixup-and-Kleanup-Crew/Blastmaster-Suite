#pragma once

#include "blastmaster/ProductKeyValidator.h"

#include <QWizard>
#include <QString>

class QLineEdit;
class QLabel;
class QProgressBar;

class SetupWizard final : public QWizard
{
    Q_OBJECT

public:
    explicit SetupWizard(QWidget* parent = nullptr);

    blastmaster::Edition detectedEdition() const;

protected:
    bool validateCurrentPage() override;

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
    void updateProductKeyStatus();
    QString editionName() const;

    QLineEdit* productKeyEdit_ = nullptr;
    QLabel* keyStatusLabel_ = nullptr;

    QLineEdit* destinationEdit_ = nullptr;

    QLabel* editionLabel_ = nullptr;
    QLabel* readyLabel_ = nullptr;

    QProgressBar* progressBar_ = nullptr;

    blastmaster::Edition detectedEdition_ =
        blastmaster::Edition::Invalid;
};
