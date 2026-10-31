#pragma once

#include <string>
#include <string_view>

namespace workbench {
namespace core {

struct VersionInfo {
    int major{0};
    int minor{1};
    int patch{0};
    std::string_view release_type{"prototype"};
};

/// Returns the semantic version of workbench_core.
VersionInfo get_version() noexcept;

/// Formatted version string: "vX.Y.Z-release_type".
std::string get_version_string();

/// Returns true if the core is initialized and self-checks pass.
bool self_check() noexcept;

} // namespace core
} // namespace workbench

