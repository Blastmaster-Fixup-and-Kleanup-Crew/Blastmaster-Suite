#include "Workbook.h"

#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QDebug>
#include <QRegularExpression>
#include <QSet>
#include <QtMath>
#include <functional>

namespace blastmaster::spreadsheet {

Workbook::Workbook()
    : m_title("Book1")
    , m_filePath()
    , m_isDirty(false)
{
    ensureSheet("Sheet1");
}

Workbook::Workbook(const QString& filePath)
    : m_title("Book1")
    , m_filePath(filePath)
    , m_isDirty(false)
{
    ensureSheet("Sheet1");
}

QString Workbook::cell(const QString& sheet, const QString& address) const
{
    return m_cells.value(sheet).value(address);
}

void Workbook::setCell(const QString& sheet, const QString& address, const QString& value)
{
    ensureSheet(sheet);
    if (m_cells[sheet].value(address) == value)
        return;

    if (value.isEmpty())
        m_cells[sheet].remove(address);
    else
        m_cells[sheet][address] = value;

    m_isDirty = true;
}

QStringList Workbook::sheets() const
{
    return m_cells.keys();
}

void Workbook::ensureSheet(const QString& sheet)
{
    if (sheet.isEmpty())
        return;
    if (!m_cells.contains(sheet))
        m_cells.insert(sheet, {});
}

bool Workbook::addSheet(const QString& sheet)
{
    const QString name = sheet.trimmed();
    if (name.isEmpty() || m_cells.contains(name))
        return false;
    m_cells.insert(name, {});
    m_isDirty = true;
    return true;
}

bool Workbook::removeSheet(const QString& sheet)
{
    if (!m_cells.contains(sheet) || m_cells.size() <= 1)
        return false;
    m_cells.remove(sheet);
    m_isDirty = true;
    return true;
}

QString Workbook::evaluateCell(const QString& sheet, const QString& address) const
{
    QSet<QString> visiting;
    std::function<QString(const QString&, const QString&)> eval =
        [&](const QString& currentSheet, const QString& currentAddress) -> QString {
            const QString key = currentSheet + QStringLiteral("!") + currentAddress.toUpper();
            if (visiting.contains(key))
                return QStringLiteral("#CIRC!");
            visiting.insert(key);
            const QString raw = cell(currentSheet, currentAddress).trimmed();
            if (!raw.startsWith('='))
                return raw;
            QString expr = raw.mid(1).trimmed();
            const auto refPattern = QRegularExpression(QStringLiteral(R"((?:(\w+)!)?([A-Z]+[0-9]+))"));
            QRegularExpressionMatchIterator it = refPattern.globalMatch(expr);
            while (it.hasNext()) {
                const auto match = it.next();
                const QString refSheet = match.captured(1).isEmpty() ? currentSheet : match.captured(1);
                const QString refAddr = match.captured(2);
                bool ok = false;
                const double value = eval(refSheet, refAddr).toDouble(&ok);
                if (!ok) {
                    visiting.remove(key);
                    return QStringLiteral("#VALUE!");
                }
                expr.replace(match.captured(0), QString::number(value, 'g', 15));
            }
            const auto terms = expr.split(QRegularExpression(QStringLiteral(R"((\+|-|\*|/))")), Qt::SkipEmptyParts);
            if (terms.size() == 1) {
                bool ok = false;
                const double n = terms.first().trimmed().toDouble(&ok);
                visiting.remove(key);
                return ok ? QString::number(n, 'g', 15) : QStringLiteral("#VALUE!");
            }
            double result = terms.first().trimmed().toDouble();
            const QRegularExpression ops(QStringLiteral(R"((\+|-|\*|/))"));
            auto opMatches = ops.globalMatch(expr);
            int i = 1;
            while (opMatches.hasNext() && i < terms.size()) {
                const QString op = opMatches.next().captured(0);
                const double rhs = terms.at(i++).trimmed().toDouble();
                if (op == "+") result += rhs;
                else if (op == "-") result -= rhs;
                else if (op == "*") result *= rhs;
                else if (op == "/") {
                    if (qFuzzyIsNull(rhs)) { visiting.remove(key); return QStringLiteral("#DIV/0!"); }
                    result /= rhs;
                }
            }
            visiting.remove(key);
            return QString::number(result, 'g', 15);
        };
    return eval(sheet, address);
}

QJsonObject Workbook::toJson() const
{
    QJsonObject root;
    root["format"] = "Blastmaster.Workbook";
    root["version"] = "1.0";
    root["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["title"] = m_title;

    QJsonArray sheets;
    for (auto it = m_cells.cbegin(); it != m_cells.cend(); ++it) {
        QJsonObject sheet;
        sheet["name"] = it.key();

        QJsonObject cells;
        for (auto cellIt = it.value().cbegin(); cellIt != it.value().cend(); ++cellIt)
            cells[cellIt.key()] = cellIt.value();

        sheet["cells"] = cells;
        sheets.append(sheet);
    }
    root["sheets"] = sheets;
    return root;
}

void Workbook::fromJson(const QJsonObject& json)
{
    m_cells.clear();

    if (json.contains("title"))
        m_title = json["title"].toString();

    const QJsonValue sheetsValue = json.value("sheets");
    if (sheetsValue.isArray()) {
        for (const QJsonValue& value : sheetsValue.toArray()) {
            const QJsonObject sheet = value.toObject();
            const QString name = sheet.value("name").toString();
            if (name.isEmpty())
                continue;

            ensureSheet(name);
            const QJsonObject cells = sheet.value("cells").toObject();
            for (auto it = cells.begin(); it != cells.end(); ++it)
                m_cells[name].insert(it.key(), it.value().toString());
        }
    } else if (sheetsValue.isObject()) {
        // Accept the original prototype's empty/object-shaped sheets field.
        ensureSheet("Sheet1");
    }

    if (m_cells.isEmpty())
        ensureSheet("Sheet1");
}

bool Workbook::save()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Workbook::save - empty file path";
        return false;
    }
    return saveAs(m_filePath);
}

bool Workbook::saveAs(const QString& filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "Workbook::saveAs - empty path";
        return false;
    }

    QString path = filePath;
    if (!path.endsWith(fileExtension(), Qt::CaseInsensitive))
        path += fileExtension();

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    const QByteArray bytes = QJsonDocument(toJson()).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size()) {
        qWarning() << "Failed to write workbook data";
        file.cancelWriting();
        return false;
    }

    if (!file.commit()) {
        qWarning() << "Failed to commit workbook:" << path;
        return false;
    }

    m_filePath = path;
    m_isDirty = false;
    qDebug() << "Workbook saved to" << path;
    return true;
}

bool Workbook::load()
{
    if (m_filePath.isEmpty()) {
        qWarning() << "Workbook::load - empty path";
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << m_filePath;
        return false;
    }

    const QByteArray bytes = file.readAll();
    file.close();

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(bytes, &error);
    if (!doc.isObject()) {
        qWarning() << "Invalid workbook format:" << error.errorString();
        return false;
    }

    fromJson(doc.object());
    m_isDirty = false;
    qDebug() << "Workbook loaded from" << m_filePath;
    return true;
}

} // namespace blastmaster::spreadsheet
