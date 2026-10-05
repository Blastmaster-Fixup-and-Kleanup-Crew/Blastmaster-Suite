#pragma once

#include <string>

namespace blastmaster {

enum class Edition {
    Standard,
    Professional
};

class ProductKeyValidator {
public:
    explicit ProductKeyValidator(Edition edition);

    bool validate(const std::string& key) const;
    std::string edition_name() const;

private:
    static std::string normalize_key(const std::string& key);

    Edition edition_;
};

} // namespace blastmaster
