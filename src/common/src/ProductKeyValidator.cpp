#include "blastmaster/ProductKeyValidator.h"

#include <cctype>
#include <string>

namespace blastmaster {

ProductKeyValidator::ProductKeyValidator(Edition edition)
    : edition_(edition) {}

std::string ProductKeyValidator::normalize_key(const std::string& key) {
    std::string normalized;
    normalized.reserve(key.size());

    for (char ch : key) {
        unsigned char value = static_cast<unsigned char>(ch);
        if (std::isalnum(value)) {
            normalized.push_back(static_cast<char>(std::toupper(value)));
        }
    }

    return normalized;
}

bool ProductKeyValidator::validate(const std::string& key) const {
    const std::string normalized = normalize_key(key);
    if (normalized.empty()) {
        return false;
    }

    if (edition_ == Edition::Standard) {
        return normalized.rfind("BMSSTD", 0) == 0 || normalized.rfind("BMSSTANDARD", 0) == 0;
    }

    return normalized.rfind("BMSPRO", 0) == 0 || normalized.rfind("BMSPROFESSIONAL", 0) == 0;
}

std::string ProductKeyValidator::edition_name() const {
    return edition_ == Edition::Standard ? "Standard" : "Professional";
}

} // namespace blastmaster
