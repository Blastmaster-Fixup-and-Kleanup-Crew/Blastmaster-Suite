#pragma once

#include "blastmaster/ProductKeyValidator.h"

#include <QString>

namespace WindowsIntegration
{
bool install(
    const QString& destination,
    blastmaster::Edition edition,
    QString& error);

bool uninstall(
    const QString& destination,
    QString& error);
}
