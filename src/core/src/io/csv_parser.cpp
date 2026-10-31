#include "workbench/io/csv_parser.hpp"
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <limits>

namespace workbench {
namespace io {

namespace {

std::string trim(std::string_view s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return std::string(s.substr(start, end - start));
}

std::string to_lower(std::string_view s) {
    std::string res;
    res.reserve(s.size());
    for (char c : s) {
        res.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return res;
}

std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::string cell;
    std::istringstream stream(line);
    while (std::getline(stream, cell, ',')) {
        cells.push_back(trim(cell));
    }
    // Handle trailing comma if line ends with comma
    if (!line.empty() && line.back() == ',') {
        cells.push_back("");
    }
    return cells;
}

} // namespace

ParseResult<domain::TemperatureField> parse_temperature_csv_string(
    std::string_view content,
    const CsvParseLimits& limits
) {
    if (content.empty()) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::EmptyFile,
            "CSV file is empty or contains no readable data."
        );
    }

    if (content.size() > limits.max_file_size_bytes) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::FileTooLarge,
            "CSV file size exceeds configured limit of " + std::to_string(limits.max_file_size_bytes) + " bytes."
        );
    }

    std::istringstream stream{std::string(content)};
    std::string line;
    int line_number = 0;

    // 1. Read header
    std::vector<std::string> header_cols;
    while (std::getline(stream, line)) {
        ++line_number;
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        header_cols = split_csv_line(trimmed);
        break;
    }

    if (header_cols.empty()) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::EmptyFile,
            "No header row found in CSV file."
        );
    }

    int idx_id = -1;
    int idx_x = -1;
    int idx_y = -1;
    int idx_z = -1;
    int idx_temp = -1;

    for (size_t i = 0; i < header_cols.size(); ++i) {
        std::string col = to_lower(header_cols[i]);
        if (col == "node_id" || col == "nodenumber" || col == "node") {
            idx_id = static_cast<int>(i);
        } else if (col == "x_m" || col == "x") {
            idx_x = static_cast<int>(i);
        } else if (col == "y_m" || col == "y") {
            idx_y = static_cast<int>(i);
        } else if (col == "z_m" || col == "z") {
            idx_z = static_cast<int>(i);
        } else if (col == "temperature_c" || col == "temperature" || col == "temp") {
            idx_temp = static_cast<int>(i);
        }
    }

    if (idx_id < 0) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::MissingColumn,
            "Missing required CSV column: 'node_id'.",
            line_number
        );
    }
    if (idx_x < 0) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::MissingColumn,
            "Missing required CSV column: 'x_m'.",
            line_number
        );
    }
    if (idx_y < 0) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::MissingColumn,
            "Missing required CSV column: 'y_m'.",
            line_number
        );
    }
    if (idx_z < 0) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::MissingColumn,
            "Missing required CSV column: 'z_m'.",
            line_number
        );
    }
    if (idx_temp < 0) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::MissingColumn,
            "Missing required CSV column: 'temperature_C'.",
            line_number
        );
    }

    domain::TemperatureField field;
    field.is_synthetic = true;
    field.source_description = "Synthetic nodal temperature dataset (self-created prototype model)";
    field.min_bounds = { std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::infinity() };
    field.max_bounds = { -std::numeric_limits<double>::infinity(),
                         -std::numeric_limits<double>::infinity(),
                         -std::numeric_limits<double>::infinity() };
    field.min_temperature_C = std::numeric_limits<double>::infinity();
    field.max_temperature_C = -std::numeric_limits<double>::infinity();

    std::unordered_set<int> seen_ids;

    while (std::getline(stream, line)) {
        ++line_number;
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;

        auto cells = split_csv_line(trimmed);
        if (cells.size() != header_cols.size()) {
            return ParseResult<domain::TemperatureField>::error(
                ParseStatusCode::MalformedRecord,
                "Row has " + std::to_string(cells.size()) + " fields, expected " + 
                std::to_string(header_cols.size()) + ".",
                line_number
            );
        }

        // Parse node_id
        int node_id = 0;
        try {
            size_t pos = 0;
            node_id = std::stoi(cells[idx_id], &pos);
            if (pos != cells[idx_id].size()) {
                throw std::invalid_argument("trailing characters");
            }
        } catch (...) {
            return ParseResult<domain::TemperatureField>::error(
                ParseStatusCode::MalformedRecord,
                "Row " + std::to_string(line_number) + ", column 'node_id' is not a valid integer: '" + cells[idx_id] + "'",
                line_number, idx_id + 1
            );
        }

        if (node_id < 0) {
            return ParseResult<domain::TemperatureField>::error(
                ParseStatusCode::MalformedRecord,
                "Node ID must be a non-negative integer. Found: " + std::to_string(node_id),
                line_number, idx_id + 1
            );
        }

        if (seen_ids.find(node_id) != seen_ids.end()) {
            return ParseResult<domain::TemperatureField>::error(
                ParseStatusCode::DuplicateIdentifier,
                "Node ID " + std::to_string(node_id) + " is repeated at row " + std::to_string(line_number) + ".",
                line_number, idx_id + 1
            );
        }
        seen_ids.insert(node_id);

        // Parse coordinates and temperature
        auto parse_double = [&](const std::string& str, const std::string& name, int col_idx, double& out) -> std::optional<std::string> {
            try {
                size_t pos = 0;
                out = std::stod(str, &pos);
                if (pos != str.size()) {
                    return "Row " + std::to_string(line_number) + ", column '" + name + "' contains invalid trailing characters.";
                }
                if (!std::isfinite(out)) {
                    return "Row " + std::to_string(line_number) + ", column '" + name + "' is not a finite number.";
                }
            } catch (...) {
                return "Row " + std::to_string(line_number) + ", column '" + name + "' is not a valid number: '" + str + "'";
            }
            return std::nullopt;
        };

        double x = 0.0, y = 0.0, z = 0.0, temp = 0.0;
        if (auto err = parse_double(cells[idx_x], "x_m", idx_x + 1, x)) {
            return ParseResult<domain::TemperatureField>::error(ParseStatusCode::NonFiniteNumber, *err, line_number, idx_x + 1);
        }
        if (auto err = parse_double(cells[idx_y], "y_m", idx_y + 1, y)) {
            return ParseResult<domain::TemperatureField>::error(ParseStatusCode::NonFiniteNumber, *err, line_number, idx_y + 1);
        }
        if (auto err = parse_double(cells[idx_z], "z_m", idx_z + 1, z)) {
            return ParseResult<domain::TemperatureField>::error(ParseStatusCode::NonFiniteNumber, *err, line_number, idx_z + 1);
        }
        if (auto err = parse_double(cells[idx_temp], "temperature_C", idx_temp + 1, temp)) {
            return ParseResult<domain::TemperatureField>::error(ParseStatusCode::NonFiniteNumber, *err, line_number, idx_temp + 1);
        }

        if (field.nodes.size() >= limits.max_nodes) {
            return ParseResult<domain::TemperatureField>::error(
                ParseStatusCode::ExceededRecordLimit,
                "Node count exceeds configured limit of " + std::to_string(limits.max_nodes),
                line_number
            );
        }

        field.nodes.push_back({node_id, x, y, z, temp});

        field.min_bounds.x = std::min(field.min_bounds.x, x);
        field.min_bounds.y = std::min(field.min_bounds.y, y);
        field.min_bounds.z = std::min(field.min_bounds.z, z);
        field.max_bounds.x = std::max(field.max_bounds.x, x);
        field.max_bounds.y = std::max(field.max_bounds.y, y);
        field.max_bounds.z = std::max(field.max_bounds.z, z);

        field.min_temperature_C = std::min(field.min_temperature_C, temp);
        field.max_temperature_C = std::max(field.max_temperature_C, temp);
    }

    if (field.nodes.empty()) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::EmptyFile,
            "The CSV contains a header but no data rows."
        );
    }

    return ParseResult<domain::TemperatureField>::success(std::move(field));
}

ParseResult<domain::TemperatureField> parse_temperature_csv_file(
    const std::filesystem::path& file_path,
    const CsvParseLimits& limits
) {
    if (!std::filesystem::exists(file_path)) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::FileNotFound,
            "File does not exist: " + file_path.string()
        );
    }

    auto file_size = std::filesystem::file_size(file_path);
    if (file_size == 0) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::EmptyFile,
            "File is 0 bytes: " + file_path.string()
        );
    }
    if (file_size > limits.max_file_size_bytes) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::FileTooLarge,
            "File size exceeds maximum configured limit of " + std::to_string(limits.max_file_size_bytes) + " bytes."
        );
    }

    std::ifstream file(file_path);
    if (!file.is_open()) {
        return ParseResult<domain::TemperatureField>::error(
            ParseStatusCode::FileNotFound,
            "Could not open file for reading: " + file_path.string()
        );
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parse_temperature_csv_string(content, limits);
}

} // namespace io
} // namespace workbench

