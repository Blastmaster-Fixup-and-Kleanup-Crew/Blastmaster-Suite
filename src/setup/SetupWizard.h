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
    QWizardPage* createProductKeyPage();
    QWizardPage* createDestinationPage();
    QWizardPage* createReadyPage();
    QWizardPage* createInstallPage();
    QWizardPage* createFinishedPage();

    bool validateProductKey();
    void updateProductKeyStatus();
    void formatProductKey();
    bool validateDestination();
    QString editionName() const;

    QLineEdit* productKeyEdit_ = nullptr;
    QLabel* keyStatusLabel_ = nullptr;

    QLineEdit* destinationEdit_ = nullptr;
    QLabel* destinationStatusLabel_ = nullptr;

    QLabel* readyLabel_ = nullptr;
    QProgressBar* progressBar_ = nullptr;

    blastmaster::Edition detectedEdition_ =
        blastmaster::Edition::Invalid;

    bool installSucceeded_ = false;
    QString installedLocation_;
};
