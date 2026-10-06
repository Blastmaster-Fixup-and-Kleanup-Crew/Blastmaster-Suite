#pragma once

#include <string>

namespace blastmaster {

enum class Edition {
    Invalid,
    Standard,
    Professional
};

class ProductKeyValidator {
public:
    explicit ProductKeyValidator(Edition edition);

    bool validate(const std::string& key) const;

    static Edition detect_edition(const std::string& key);

    std::string edition_name() const;

    Edition edition() const;

private:
    static std::string normalize_key(const std::string& key);

    Edition edition_;
};

} // namespace blastmaster
