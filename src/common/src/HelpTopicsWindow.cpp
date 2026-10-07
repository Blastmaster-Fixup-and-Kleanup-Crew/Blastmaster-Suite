#include "blastmaster/HelpTopicsWindow.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextDocument>
#include <QUrl>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>

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
    {"Help Videos", ":/help/help-videos.md", "suite"},

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
    , m_searchEdit(new QLineEdit(this))
    , m_statusLabel(new QLabel(this))
{
    setWindowTitle(displayName(applicationId) + " - Help Topics");
    resize(900, 620);

    auto* layout = new QVBoxLayout(this);
    auto* content = new QHBoxLayout();

    auto* searchRow = new QHBoxLayout();
    auto* searchLabel = new QLabel(tr("&Search topics:"), this);
    m_searchEdit->setPlaceholderText(tr("Type a topic name..."));
    m_searchEdit->setAccessibleName(tr("Search help topics"));
    searchLabel->setBuddy(m_searchEdit);
    searchRow->addWidget(searchLabel);
    searchRow->addWidget(m_searchEdit, 1);
    layout->addLayout(searchRow);

    m_topicList->setMinimumWidth(240);
    m_topicList->setAccessibleName(tr("Help topics"));
    m_topicList->setFocusPolicy(Qt::StrongFocus);

    m_topicView->setOpenExternalLinks(false);
    m_topicView->setReadOnly(true);
    m_topicView->setAccessibleName(tr("Help topic content"));

    content->addWidget(m_topicList);
    content->addWidget(m_topicView, 1);
    layout->addLayout(content, 1);

    auto* bottomRow = new QHBoxLayout();
    m_statusLabel->setAccessibleName(tr("Help topic status"));
    bottomRow->addWidget(m_statusLabel, 1);

    auto* closeButton = new QPushButton(tr("Close"), this);
    closeButton->setDefault(true);
    closeButton->setAutoDefault(true);
    closeButton->setFocusPolicy(Qt::StrongFocus);
    QObject::connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    bottomRow->addWidget(closeButton);
    layout->addLayout(bottomRow);

    setTabOrder(m_searchEdit, m_topicList);
    setTabOrder(m_topicList, m_topicView);
    setTabOrder(m_topicView, closeButton);

    auto* findShortcut = new QShortcut(QKeySequence::Find, this);
    QObject::connect(findShortcut, &QShortcut::activated, this, [this]() {
        m_searchEdit->setFocus();
        m_searchEdit->selectAll();
    });

    QObject::connect(m_searchEdit, &QLineEdit::textChanged,
                     this, &HelpTopicsWindow::filterTopics);
    QObject::connect(m_topicList, &QListWidget::currentItemChanged,
                     this, [this](QListWidgetItem*) { showCurrentTopic(); });
    QObject::connect(m_topicView, &QTextBrowser::anchorClicked,
                     this, &HelpTopicsWindow::activateTopicLink);

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

    if (m_topicList->count() > 0) {
        m_topicList->setCurrentRow(0);
        m_topicList->setFocus();
    }
    filterTopics(QString());
}

void HelpTopicsWindow::filterTopics(const QString& text)
{
    const QString query = text.trimmed();
    int visibleCount = 0;
    QListWidgetItem* firstVisible = nullptr;

    for (int i = 0; i < m_topicList->count(); ++i) {
        auto* item = m_topicList->item(i);
        const bool matches = query.isEmpty() ||
            item->text().contains(query, Qt::CaseInsensitive);
        item->setHidden(!matches);
        if (matches) {
            ++visibleCount;
            if (!firstVisible) firstVisible = item;
        }
    }

    m_statusLabel->setText(
        visibleCount == 1
            ? tr("1 topic")
            : tr("%1 topics").arg(visibleCount));

    if (firstVisible) {
        if (!m_topicList->currentItem() || m_topicList->currentItem()->isHidden()) {
            m_topicList->setCurrentItem(firstVisible);
        }
    } else {
        m_topicView->setMarkdown(tr("# No matching topics\n\nTry a different search term."));
    }
}

void HelpTopicsWindow::showCurrentTopic()
{
    auto* current = m_topicList->currentItem();
    if (!current || current->isHidden()) {
        return;
    }

    m_topicView->document()->setMarkdown(
        loadMarkdown(current->data(Qt::UserRole).toString()));
}

void HelpTopicsWindow::activateTopicLink(const QUrl& url)
{
    if (!url.isValid()) {
        return;
    }

    const QString target = url.path().section('/', -1);
    for (int i = 0; i < m_topicList->count(); ++i) {
        auto* item = m_topicList->item(i);
        const QString resource = item->data(Qt::UserRole).toString();
        if (resource.endsWith(QStringLiteral("/") + target)) {
            m_searchEdit->clear();
            m_topicList->setCurrentItem(item);
            return;
        }
    }
}

} // namespace blastmaster
