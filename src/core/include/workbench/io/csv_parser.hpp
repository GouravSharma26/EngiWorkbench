#pragma once

#include "workbench/domain/geometry.hpp"
#include "workbench/io/parse_result.hpp"
#include <filesystem>
#include <string_view>

namespace workbench {
namespace io {

struct CsvParseLimits {
    size_t max_file_size_bytes{50 * 1024 * 1024}; // 50 MiB limit
    size_t max_nodes{500000};                     // 500,000 nodes limit
};

/// Parses synthetic nodal temperature CSV with required headers:
/// node_id, x_m, y_m, z_m, temperature_C (flexible order).
/// Rejects duplicate IDs, missing columns, extra values, NaN/Inf.
ParseResult<domain::TemperatureField> parse_temperature_csv_file(
    const std::filesystem::path& file_path,
    const CsvParseLimits& limits = CsvParseLimits{}
);

/// Parses CSV from an in-memory string.
ParseResult<domain::TemperatureField> parse_temperature_csv_string(
    std::string_view content,
    const CsvParseLimits& limits = CsvParseLimits{}
);

} // namespace io
} // namespace workbench
