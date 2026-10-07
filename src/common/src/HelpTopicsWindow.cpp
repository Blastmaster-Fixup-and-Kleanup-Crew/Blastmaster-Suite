#include "blastmaster/HelpTopicsWindow.h"

#include <QFile>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QApplication>

namespace blastmaster {

namespace {

struct Topic {
    const char* title;
    const char* resource;
    const char* application;
};

static const Topic kTopics[] = {
    {"Blastmaster Suite Overview", ":/help/suite-overview.md", "suite"},
    {"Installation", ":/help/installation.md", "suite"},
    {"Product Keys and Editions", ":/help/product-keys.md", "suite"},
    {"Uninstalling Blastmaster Suite", ":/help/uninstall.md", "suite"},
    {"File Formats", ":/help/file-formats.md", "suite"},
    {"Common Commands", ":/help/common-commands.md", "suite"},
    {"Keyboard Navigation", ":/help/keyboard-navigation.md", "suite"},
    {"Troubleshooting", ":/help/troubleshooting.md", "suite"},
    {"Support", ":/help/support.md", "suite"},

    {"Docs Help", ":/help/docs.md", "docs"},
    {"Workbooks Help", ":/help/workbooks.md", "workbooks"},
    {"Presentations Help", ":/help/presentations.md", "presentations"},
    {"Databases Help", ":/help/databases.md", "databases"},
};

static QString displayName(const QString& applicationId)
{
    if (applicationId == "docs") return "Blastmaster Docs";
    if (applicationId == "workbooks") return "Blastmaster Workbooks";
    if (applicationId == "presentations") return "Blastmaster Presentations";
    if (applicationId == "databases") return "Blastmaster Databases";
    return "Blastmaster Suite";
}

static QString loadMarkdown(const QString& resource)
{
    QFile file(resource);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringLiteral("# Help Topic Unavailable\n\nUnable to load this help topic.");
    }
    return QString::fromUtf8(file.readAll());
}

} // namespace

void HelpTopicsWindow::showFor(QWidget* parent, const QString& applicationId)
{
    auto* dialog = new HelpTopicsWindow(parent, applicationId);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

HelpTopicsWindow::HelpTopicsWindow(QWidget* parent, const QString& applicationId)
    : QDialog(parent)
    , m_applicationId(applicationId)
    , m_topicList(new QListWidget(this))
    , m_topicView(new QTextBrowser(this))
{
    setWindowTitle(displayName(applicationId) + " - Help Topics");
    resize(900, 620);

    auto* layout = new QVBoxLayout(this);
    auto* content = new QHBoxLayout();

    m_topicList->setMinimumWidth(240);
    m_topicView->setOpenExternalLinks(false);
    m_topicView->setReadOnly(true);

    content->addWidget(m_topicList);
    content->addWidget(m_topicView, 1);
    layout->addLayout(content, 1);

    auto* closeButton = new QPushButton(tr("Close"), this);
    closeButton->setDefault(true);
    QObject::connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignRight);

    loadTopics();
}

void HelpTopicsWindow::loadTopics()
{
    const QString appTopic =
        m_applicationId == "docs" ? "docs" :
        m_applicationId == "workbooks" ? "workbooks" :
        m_applicationId == "presentations" ? "presentations" :
        m_applicationId == "databases" ? "databases" : "";

    for (const auto& topic : kTopics) {
        if (QString::fromLatin1(topic.application) != "suite" &&
            QString::fromLatin1(topic.application) != appTopic) {
            continue;
        }

        auto* item = new QListWidgetItem(QString::fromLatin1(topic.title), m_topicList);
        item->setData(Qt::UserRole, QString::fromLatin1(topic.resource));
    }

    QObject::connect(m_topicList, &QListWidget::currentItemChanged,
                     this, [this](QListWidgetItem* current) {
        if (!current) {
            m_topicView->clear();
            return;
        }
        m_topicView->document()->setMarkdown(
            loadMarkdown(current->data(Qt::UserRole).toString()));
    });

    if (m_topicList->count() > 0) {
        m_topicList->setCurrentRow(0);
    }
}

} // namespace blastmaster
