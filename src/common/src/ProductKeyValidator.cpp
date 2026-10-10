#include "blastmaster/ProductKeyValidator.h"

#include <cctype>
#include <string>

namespace blastmaster {

namespace {

constexpr const char* STANDARD_KEY =
    "CHKZMF0MKMPWRKIRVYXIV9SVY";

constexpr const char* PROFESSIONAL_KEY =
    "JLGCDIQR9ZTNXR79BFUB9OELD";

} // namespace

ProductKeyValidator::ProductKeyValidator(Edition edition)
    : edition_(edition)
{
}

std::string ProductKeyValidator::normalize_key(const std::string& key)
{
    std::string normalized;
    normalized.reserve(key.size());

    for (char ch : key)
    {
        const unsigned char value =
            static_cast<unsigned char>(ch);

        if (std::isalnum(value))
        {
            normalized.push_back(
                static_cast<char>(std::toupper(value)));
        }
    }

    return normalized;
}

bool ProductKeyValidator::validate(const std::string& key) const
{
    const std::string normalized =
        normalize_key(key);

    switch (edition_)
    {
    case Edition::Standard:
        return normalized == STANDARD_KEY;

    case Edition::Professional:
        return normalized == PROFESSIONAL_KEY;

    case Edition::Invalid:
    default:
        return false;
    }
}

Edition ProductKeyValidator::detect_edition(
    const std::string& key)
{
    const std::string normalized =
        normalize_key(key);

    if (normalized == STANDARD_KEY)
        return Edition::Standard;

    if (normalized == PROFESSIONAL_KEY)
        return Edition::Professional;

    return Edition::Invalid;
}

std::string ProductKeyValidator::edition_name() const
{
    switch (edition_)
    {
    case Edition::Standard:
        return "Standard";

    case Edition::Professional:
        return "Professional";

    case Edition::Invalid:
    default:
        return "Invalid";
    }
}

Edition ProductKeyValidator::edition() const
{
    return edition_;
}

} // namespace blastmaster
