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
#include <QFrame>
#include <QFont>
#include <QToolButton>
#include <QSignalBlocker>

// Ensure resources in this shared library are registered even when the
// linker would otherwise discard the generated qrc object from a static link.
static void initializeBlastmasterHelpResources()
{
    Q_INIT_RESOURCE(help);
}

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
        return QStringLiteral("# Help Topic Unavailable\n\nCould not open bundled resource: %1. Rebuild/reinstall Blastmaster Suite so help resources are included.")
            .arg(resource.toHtmlEscaped());
    }
    const QString markdown = QString::fromUtf8(file.readAll()).trimmed();
    if (markdown.isEmpty()) {
        return QStringLiteral("# Help Topic Empty\n\nThe bundled help topic %1 contains no content.")
            .arg(resource.toHtmlEscaped());
    }
    return markdown;
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
    initializeBlastmasterHelpResources();
    setWindowTitle(displayName(applicationId) + " Help");
    setMinimumSize(720, 480);
    resize(900, 620);

    // Classic Windows-era help styling, with Blastmaster branding.
    setStyleSheet(QStringLiteral(R"(
        QDialog { background: #d4d0c8; color: #000000; }
        QLabel { color: #000000; }
        QLineEdit, QListWidget, QTextBrowser {
            background: #ffffff; color: #000000;
            border: 1px solid #808080;
            selection-background-color: #000080;
            selection-color: #ffffff;
        }
        QPushButton {
            background: #d4d0c8; color: #000000;
            border: 1px solid #808080;
            border-top-color: #ffffff; border-left-color: #ffffff;
            padding: 4px 14px; min-width: 64px;
        }
        QPushButton:pressed {
            border-top-color: #808080; border-left-color: #808080;
            border-bottom-color: #ffffff; border-right-color: #ffffff;
        }
        QListWidget::item { padding: 4px 5px; }
        QListWidget::item:selected { background: #000080; color: #ffffff; }
    )"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 8);
    layout->setSpacing(8);

    auto* brandHeader = new QFrame(this);
    brandHeader->setFrameShape(QFrame::StyledPanel);
    brandHeader->setStyleSheet(QStringLiteral(
        "QFrame { background: #000080; border: 1px solid #000040; }"));
    auto* brandLayout = new QVBoxLayout(brandHeader);
    brandLayout->setContentsMargins(10, 7, 10, 7);
    brandLayout->setSpacing(2);
    auto* brandTitle = new QLabel(displayName(applicationId) + tr(" Help"), brandHeader);
    QFont brandFont = brandTitle->font();
    brandFont.setBold(true);
    brandFont.setPointSize(12);
    brandTitle->setFont(brandFont);
    brandTitle->setStyleSheet(QStringLiteral("color: #ffffff;"));
    auto* brandSubtitle = new QLabel(
        tr("Help Topics  |  Blastmaster Suite  |  Version 1.0 Milestone 1 Build 67"),
        brandHeader);
    brandSubtitle->setStyleSheet(QStringLiteral("color: #e8e8ff;"));
    brandLayout->addWidget(brandTitle);
    brandLayout->addWidget(brandSubtitle);
    layout->addWidget(brandHeader);

    auto* navigationRow = new QHBoxLayout();
    navigationRow->setSpacing(4);
    auto* contentsButton = new QPushButton(tr("&Contents"), this);
    contentsButton->setToolTip(tr("Show all help topics"));
    auto* indexButton = new QPushButton(tr("&Index"), this);
    indexButton->setToolTip(tr("Find topics by keyword"));
    m_backButton = new QPushButton(tr("< &Back"), this);
    m_backButton->setToolTip(tr("Return to the previously viewed topic"));
    m_forwardButton = new QPushButton(tr("&Forward >"), this);
    m_forwardButton->setToolTip(tr("Return to the next topic in browsing history"));
    m_backButton->setEnabled(false);
    m_forwardButton->setEnabled(false);
    navigationRow->addWidget(contentsButton);
    navigationRow->addWidget(indexButton);
    navigationRow->addStretch(1);
    navigationRow->addWidget(m_backButton);
    navigationRow->addWidget(m_forwardButton);
    layout->addLayout(navigationRow);

    auto* searchRow = new QHBoxLayout();
    auto* searchLabel = new QLabel(tr("&Search help:"), this);
    m_searchEdit->setPlaceholderText(tr("Search topic titles and article text..."));
    m_searchEdit->setAccessibleName(tr("Search help topics and article text"));
    searchLabel->setBuddy(m_searchEdit);
    searchRow->addWidget(searchLabel);
    searchRow->addWidget(m_searchEdit, 1);
    layout->addLayout(searchRow);

    m_topicList->setMinimumWidth(240);
    m_topicList->setMaximumWidth(320);
    m_topicList->setAccessibleName(tr("Help topics"));
    m_topicList->setFocusPolicy(Qt::StrongFocus);

    m_topicView->setOpenExternalLinks(false);
    m_topicView->setOpenLinks(true);
    m_topicView->setReadOnly(true);
    m_topicView->setAccessibleName(tr("Help topic content"));
    m_topicView->document()->setDefaultStyleSheet(
        QStringLiteral("body { font-family: Arial, sans-serif; font-size: 10pt; color: #202020; }"
                       "h1 { color: #000080; border-bottom: 1px solid #808080; padding-bottom: 4px; }"
                       "h2 { color: #000080; margin-top: 16px; }"
                       "a { color: #000080; }"
                       "table { border-collapse: collapse; }"
                       "th, td { border: 1px solid #808080; padding: 4px 7px; }"));

    auto* content = new QHBoxLayout();
    content->setSpacing(8);
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

    QObject::connect(contentsButton, &QPushButton::clicked, this, [this]() {
        m_searchEdit->clear();
        m_topicList->setFocus();
        if (m_topicList->count() > 0) m_topicList->setCurrentRow(0);
    });
    QObject::connect(indexButton, &QPushButton::clicked, this, [this]() {
        m_searchEdit->setFocus();
        m_searchEdit->selectAll();
    });
    QObject::connect(m_backButton, &QPushButton::clicked, this, [this]() {
        navigateHistory(-1);
    });
    QObject::connect(m_forwardButton, &QPushButton::clicked, this, [this]() {
        navigateHistory(1);
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
        const QString resource = item->data(Qt::UserRole).toString();
        const bool matches = query.isEmpty() ||
            item->text().contains(query, Qt::CaseInsensitive) ||
            loadMarkdown(resource).contains(query, Qt::CaseInsensitive);
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

    const int row = m_topicList->row(current);
    if (!m_navigatingHistory) {
        if (m_historyPosition >= 0 && m_historyPosition < m_topicHistory.size() - 1) {
            m_topicHistory.resize(m_historyPosition + 1);
        }
        if (m_topicHistory.isEmpty() || m_topicHistory.last() != row) {
            m_topicHistory.push_back(row);
        }
        m_historyPosition = m_topicHistory.size() - 1;
    }

    const QString resource = current->data(Qt::UserRole).toString();
    m_topicView->document()->setMarkdown(loadMarkdown(resource));
    m_topicView->document()->setBaseUrl(QUrl(QStringLiteral("qrc:/help/")));
    m_topicView->setAccessibleDescription(current->text());
    m_statusLabel->setText(tr("Viewing: %1").arg(current->text()));
    updateNavigationButtons();
}

void HelpTopicsWindow::navigateHistory(int offset)
{
    const int target = m_historyPosition + offset;
    if (target < 0 || target >= m_topicHistory.size()) {
        return;
    }

    const int row = m_topicHistory.at(target);
    if (row < 0 || row >= m_topicList->count() || m_topicList->item(row)->isHidden()) {
        return;
    }

    m_historyPosition = target;
    m_navigatingHistory = true;
    m_topicList->setCurrentRow(row);
    m_navigatingHistory = false;
    updateNavigationButtons();
}

void HelpTopicsWindow::updateNavigationButtons()
{
    if (m_backButton) {
        m_backButton->setEnabled(m_historyPosition > 0);
    }
    if (m_forwardButton) {
        m_forwardButton->setEnabled(
            m_historyPosition >= 0 && m_historyPosition < m_topicHistory.size() - 1);
    }
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
