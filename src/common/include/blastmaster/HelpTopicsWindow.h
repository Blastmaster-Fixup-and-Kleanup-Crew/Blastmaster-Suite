#pragma once

#include <QDialog>
#include <QString>

class QListWidget;
class QTextBrowser;

namespace blastmaster {

class HelpTopicsWindow final : public QDialog
{
public:
    static void showFor(QWidget* parent, const QString& applicationId);

private:
    explicit HelpTopicsWindow(QWidget* parent, const QString& applicationId);

    void loadTopics();

    QString m_applicationId;
    QListWidget* m_topicList;
    QTextBrowser* m_topicView;
};

} // namespace blastmaster
