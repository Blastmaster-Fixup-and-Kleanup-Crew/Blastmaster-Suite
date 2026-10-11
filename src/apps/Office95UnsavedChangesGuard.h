#pragma once

#include <QEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
#include <QWidget>
#include <functional>

namespace blastmaster::ui {

// Shared close-time save prompt for document-based applications.
// The supplied callbacks keep this helper independent of each app's document model.
class UnsavedChangesGuard final : public QObject
{
public:
    UnsavedChangesGuard(QWidget* window,
                        std::function<bool()> isDirty,
                        std::function<bool()> save,
                        QObject* parent = nullptr)
        : QObject(parent ? parent : window),
          m_window(window),
          m_isDirty(std::move(isDirty)),
          m_save(std::move(save))
    {
        if (m_window)
            m_window->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (watched != m_window || event->type() != QEvent::Close ||
            !m_isDirty || !m_isDirty())
            return QObject::eventFilter(watched, event);

        const auto choice = QMessageBox::warning(
            m_window,
            QStringLiteral("Unsaved Changes"),
            QStringLiteral("Save changes before closing?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);

        if (choice == QMessageBox::Discard)
            return QObject::eventFilter(watched, event);

        if (choice == QMessageBox::Save && m_save && m_save())
            return QObject::eventFilter(watched, event);

        event->ignore();
        return true;
    }

private:
    QWidget* m_window{};
    std::function<bool()> m_isDirty;
    std::function<bool()> m_save;
};

} // namespace blastmaster::ui
