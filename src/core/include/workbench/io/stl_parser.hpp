#pragma once

#include "workbench/domain/geometry.hpp"
#include "workbench/io/parse_result.hpp"
#include <filesystem>
#include <string_view>

namespace workbench {
namespace io {

struct StlParseLimits {
    size_t max_file_size_bytes{50 * 1024 * 1024}; // 50 MiB limit
    size_t max_triangles{250000};                 // 250,000 triangles limit
};

/// Parses ASCII STL files into domain::MeshData.
/// Rejects binary STL, truncated loops, malformed vertices, and non-finite numbers.
ParseResult<domain::MeshData> parse_ascii_stl_file(
    const std::filesystem::path& file_path,
    const StlParseLimits& limits = StlParseLimits{}
);

/// Parses ASCII STL from an in-memory string.
ParseResult<domain::MeshData> parse_ascii_stl_string(
    std::string_view content,
    const StlParseLimits& limits = StlParseLimits{}
);

} // namespace io
} // namespace workbench
