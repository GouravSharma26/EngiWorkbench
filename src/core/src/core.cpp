#include "workbench/core.hpp"
#include <sstream>

namespace workbench {
namespace core {

VersionInfo get_version() noexcept {
    return VersionInfo{0, 1, 0, "prototype"};
}

std::string get_version_string() {
    auto v = get_version();
    std::ostringstream oss;
    oss << "v" << v.major << "." << v.minor << "." << v.patch << "-" << v.release_type;
    return oss.str();
}

bool self_check() noexcept {
    // Basic verification that C++17 core initializes correctly
    auto v = get_version();
    return (v.major >= 0 && v.minor >= 0 && v.patch >= 0 && !v.release_type.empty());
}

} // namespace core
} // namespace workbench

