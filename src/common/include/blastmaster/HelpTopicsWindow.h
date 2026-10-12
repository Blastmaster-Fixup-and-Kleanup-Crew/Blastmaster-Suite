#pragma once

#include <QDialog>
#include <QString>
#include <QUrl>
#include <QVector>

class QListWidget;
class QTextBrowser;
class QLineEdit;
class QLabel;

namespace blastmaster {

class HelpTopicsWindow final : public QDialog
{
public:
    static void showFor(QWidget* parent, const QString& applicationId);

private:
    explicit HelpTopicsWindow(QWidget* parent, const QString& applicationId);

    void loadTopics();
    void filterTopics(const QString& text);
    void showCurrentTopic();
    void activateTopicLink(const QUrl& url);
    void navigateHistory(int offset);

    QString m_applicationId;
    QListWidget* m_topicList;
    QTextBrowser* m_topicView;
    QLineEdit* m_searchEdit;
    QLabel* m_statusLabel;
    QVector<int> m_topicHistory;
    int m_historyPosition = -1;
    bool m_navigatingHistory = false;
};

} // namespace blastmaster
