#pragma once

#include <QMessageBox>
#include <QWidget>
#include <functional>

namespace blastmaster::ui {

// Ask before replacing the active document. A failed or cancelled save must
// leave the current document open and unchanged.
inline bool confirmDocumentReplacement(
    QWidget* parent,
    bool dirty,
    const std::function<bool()>& save)
{
    if (!dirty)
        return true;

    const auto choice = QMessageBox::warning(
        parent,
        QStringLiteral("Unsaved Changes"),
        QStringLiteral("Save changes before opening another file?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (choice == QMessageBox::Discard)
        return true;
    if (choice != QMessageBox::Save || !save)
        return false;
    return save();
}

} // namespace blastmaster::ui
