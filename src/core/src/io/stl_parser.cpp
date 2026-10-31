#include "workbench/io/stl_parser.hpp"
#include <fstream>
#include <sstream>
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

bool starts_with_case_insensitive(const std::string& str, std::string_view prefix) {
    if (str.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(str[i])) != 
            std::tolower(static_cast<unsigned char>(prefix[i]))) {
            return false;
        }
    }
    return true;
}

bool has_binary_characters(std::string_view content) {
    // Check first 512 bytes for non-text characters (null byte, control chars other than tab, CR, LF)
    size_t check_len = std::min<size_t>(content.size(), 512);
    for (size_t i = 0; i < check_len; ++i) {
        unsigned char c = static_cast<unsigned char>(content[i]);
        if (c == 0) return true;
        if (c < 9 || (c > 13 && c < 32)) return true;
    }
    return false;
}

} // namespace

ParseResult<domain::MeshData> parse_ascii_stl_string(
    std::string_view content,
    const StlParseLimits& limits
) {
    if (content.empty()) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::EmptyFile, 
            "File is empty or contains no readable data."
        );
    }

    if (content.size() > limits.max_file_size_bytes) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::FileTooLarge,
            "File size (" + std::to_string(content.size()) + " bytes) exceeds maximum configured limit of " +
            std::to_string(limits.max_file_size_bytes) + " bytes (50 MiB)."
        );
    }

    if (has_binary_characters(content)) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::BinaryStlDetected,
            "The selected file appears to be binary STL. Export it as ASCII STL and try again."
        );
    }

    std::istringstream stream{std::string(content)};
    std::string line;
    int line_number = 0;

    // Find initial 'solid' line
    bool found_solid = false;
    std::string solid_name;
    while (std::getline(stream, line)) {
        ++line_number;
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;

        if (starts_with_case_insensitive(trimmed, "solid")) {
            found_solid = true;
            if (trimmed.size() > 5) {
                solid_name = trim(trimmed.substr(5));
            }
            break;
        } else {
            return ParseResult<domain::MeshData>::error(
                ParseStatusCode::InvalidFormat,
                "File must begin with 'solid [name]'. Found: '" + trimmed + "'",
                line_number
            );
        }
    }

    if (!found_solid) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::EmptyFile,
            "No 'solid' declaration found in ASCII STL file."
        );
    }

    domain::MeshData mesh;
    mesh.solid_name = solid_name;
    mesh.min_bounds = { std::numeric_limits<double>::infinity(),
                        std::numeric_limits<double>::infinity(),
                        std::numeric_limits<double>::infinity() };
    mesh.max_bounds = { -std::numeric_limits<double>::infinity(),
                        -std::numeric_limits<double>::infinity(),
                        -std::numeric_limits<double>::infinity() };

    auto update_bounds = [&](const domain::Point3D& p) {
        mesh.min_bounds.x = std::min(mesh.min_bounds.x, p.x);
        mesh.min_bounds.y = std::min(mesh.min_bounds.y, p.y);
        mesh.min_bounds.z = std::min(mesh.min_bounds.z, p.z);
        mesh.max_bounds.x = std::max(mesh.max_bounds.x, p.x);
        mesh.max_bounds.y = std::max(mesh.max_bounds.y, p.y);
        mesh.max_bounds.z = std::max(mesh.max_bounds.z, p.z);
    };

    bool inside_facet = false;
    bool inside_loop = false;
    domain::Triangle current_tri{};
    int vertex_count = 0;
    bool found_endsolid = false;

    while (std::getline(stream, line)) {
        ++line_number;
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;

        if (starts_with_case_insensitive(trimmed, "facet normal")) {
            if (inside_facet) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Unexpected 'facet normal' inside an unclosed facet.",
                    line_number
                );
            }
            inside_facet = true;
            vertex_count = 0;

            std::istringstream ls(trimmed.substr(12));
            if (!(ls >> current_tri.normal.x >> current_tri.normal.y >> current_tri.normal.z)) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Invalid facet normal numbers.",
                    line_number
                );
            }
            if (!current_tri.normal.is_finite()) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::NonFiniteNumber,
                    "Non-finite normal coordinate encountered.",
                    line_number
                );
            }
        } else if (starts_with_case_insensitive(trimmed, "outer loop")) {
            if (!inside_facet || inside_loop) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Unexpected 'outer loop' keyword.",
                    line_number
                );
            }
            inside_loop = true;
            vertex_count = 0;
        } else if (starts_with_case_insensitive(trimmed, "vertex")) {
            if (!inside_loop) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "'vertex' keyword must be inside an 'outer loop'.",
                    line_number
                );
            }
            if (vertex_count >= 3) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Facet contains more than 3 vertices.",
                    line_number
                );
            }

            domain::Point3D pt{};
            std::istringstream ls(trimmed.substr(6));
            if (!(ls >> pt.x >> pt.y >> pt.z)) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Invalid vertex coordinates.",
                    line_number
                );
            }
            if (!pt.is_finite()) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::NonFiniteNumber,
                    "Non-finite vertex coordinate encountered.",
                    line_number
                );
            }

            if (vertex_count == 0) current_tri.v1 = pt;
            else if (vertex_count == 1) current_tri.v2 = pt;
            else if (vertex_count == 2) current_tri.v3 = pt;
            update_bounds(pt);
            ++vertex_count;
        } else if (starts_with_case_insensitive(trimmed, "endloop")) {
            if (!inside_loop || vertex_count != 3) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Facet outer loop closed with " + std::to_string(vertex_count) + " vertices (expected 3).",
                    line_number
                );
            }
            inside_loop = false;
        } else if (starts_with_case_insensitive(trimmed, "endfacet")) {
            if (!inside_facet || inside_loop || vertex_count != 3) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Premature or malformed 'endfacet'.",
                    line_number
                );
            }
            inside_facet = false;

            if (mesh.triangles.size() >= limits.max_triangles) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::ExceededRecordLimit,
                    "Exceeded configured triangle count limit of " + std::to_string(limits.max_triangles),
                    line_number
                );
            }
            mesh.triangles.push_back(current_tri);
        } else if (starts_with_case_insensitive(trimmed, "endsolid")) {
            if (inside_facet || inside_loop) {
                return ParseResult<domain::MeshData>::error(
                    ParseStatusCode::MalformedRecord,
                    "Found 'endsolid' before closing current facet.",
                    line_number
                );
            }
            found_endsolid = true;
            break;
        }
    }

    if (!found_endsolid) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::InvalidFormat,
            "File ended abruptly without 'endsolid'."
        );
    }

    if (mesh.triangles.empty()) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::EmptyFile,
            "STL file contains zero valid facets."
        );
    }

    return ParseResult<domain::MeshData>::success(std::move(mesh));
}

ParseResult<domain::MeshData> parse_ascii_stl_file(
    const std::filesystem::path& file_path,
    const StlParseLimits& limits
) {
    if (!std::filesystem::exists(file_path)) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::FileNotFound,
            "File does not exist: " + file_path.string()
        );
    }

    auto file_size = std::filesystem::file_size(file_path);
    if (file_size == 0) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::EmptyFile,
            "File is 0 bytes: " + file_path.string()
        );
    }
    if (file_size > limits.max_file_size_bytes) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::FileTooLarge,
            "File size (" + std::to_string(file_size) + " bytes) exceeds maximum limit of " +
            std::to_string(limits.max_file_size_bytes) + " bytes."
        );
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return ParseResult<domain::MeshData>::error(
            ParseStatusCode::FileNotFound,
            "Could not open file for reading: " + file_path.string()
        );
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parse_ascii_stl_string(content, limits);
}

} // namespace io
} // namespace workbench

