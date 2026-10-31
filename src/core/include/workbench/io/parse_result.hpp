#pragma once

#include <string>
#include <optional>

namespace workbench {
namespace io {

enum class ParseStatusCode {
    Ok,
    FileNotFound,
    FileTooLarge,
    EmptyFile,
    InvalidFormat,
    BinaryStlDetected,
    MissingHeader,
    MissingColumn,
    DuplicateIdentifier,
    NonFiniteNumber,
    MalformedRecord,
    ExceededRecordLimit
};

template <typename T>
struct ParseResult {
    ParseStatusCode status{ParseStatusCode::Ok};
    std::string error_message;
    int line_number{0};
    int column_number{0};
    std::optional<T> data{std::nullopt};

    bool is_ok() const noexcept {
        return status == ParseStatusCode::Ok && data.has_value();
    }

    static ParseResult<T> success(T value) {
        ParseResult<T> res;
        res.status = ParseStatusCode::Ok;
        res.data = std::move(value);
        return res;
    }

    static ParseResult<T> error(ParseStatusCode code, std::string msg, int line = 0, int col = 0) {
        ParseResult<T> res;
        res.status = code;
        res.error_message = std::move(msg);
        res.line_number = line;
        res.column_number = col;
        return res;
    }
};

} // namespace io
} // namespace workbench
