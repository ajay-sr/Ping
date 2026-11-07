#include "Config.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

namespace {

// A value in the config file: a number, or an array of numbers.
struct JsonValue {
    bool isArray = false;
    std::vector<double> numbers;  // exactly one entry for a plain number
};

// Minimal parser for a flat JSON object whose values are numbers or arrays of numbers.
class FlatJsonParser {
public:
    explicit FlatJsonParser(std::string text) : text_(std::move(text)) {}

    bool parse(std::map<std::string, JsonValue>& out) {
        skipWhitespace();
        if (!consume('{')) return fail("expected '{'");
        skipWhitespace();
        if (consume('}')) return finish();

        while (true) {
            std::string key;
            if (!parseString(key)) return false;
            skipWhitespace();
            if (!consume(':')) return fail("expected ':' after key");
            skipWhitespace();
            JsonValue value;
            if (!parseValue(value)) return false;
            out[key] = value;

            skipWhitespace();
            if (consume('}')) return finish();
            if (!consume(',')) return fail("expected ',' or '}'");
            skipWhitespace();
        }
    }

    const std::string& error() const { return error_; }

private:
    bool parseString(std::string& out) {
        if (!consume('"')) return fail("expected a quoted key");
        while (pos_ < text_.size() && text_[pos_] != '"') {
            out += text_[pos_++];
        }
        if (!consume('"')) return fail("unterminated string");
        return true;
    }

    bool parseValue(JsonValue& out) {
        if (!consume('[')) {
            out.numbers.resize(1);
            return parseNumber(out.numbers[0]);
        }
        out.isArray = true;
        skipWhitespace();
        if (consume(']')) return true;
        while (true) {
            double number = 0.0;
            if (!parseNumber(number)) return false;
            out.numbers.push_back(number);
            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return fail("expected ',' or ']'");
            skipWhitespace();
        }
    }

    bool parseNumber(double& out) {
        const char* start = text_.c_str() + pos_;
        char* end = nullptr;
        out = std::strtod(start, &end);
        if (end == start) return fail("expected a number");
        pos_ += static_cast<std::size_t>(end - start);
        return true;
    }

    bool finish() {
        skipWhitespace();
        if (pos_ != text_.size()) return fail("unexpected text after '}'");
        return true;
    }

    void skipWhitespace() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    bool consume(char c) {
        if (pos_ < text_.size() && text_[pos_] == c) {
            ++pos_;
            return true;
        }
        return false;
    }

    bool fail(const std::string& message) {
        error_ = message + " at offset " + std::to_string(pos_);
        return false;
    }

    std::string text_;
    std::size_t pos_ = 0;
    std::string error_;
};

}  // namespace

bool loadConfig(const std::string& path, Config& config) {
    std::ifstream file(path);
    if (!file) {
        std::fprintf(stderr, "No config file at %s, using defaults\n", path.c_str());
        return true;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::map<std::string, JsonValue> values;
    FlatJsonParser parser(buffer.str());
    if (!parser.parse(values)) {
        std::fprintf(stderr, "%s: %s\n", path.c_str(), parser.error().c_str());
        return false;
    }

    bool ok = true;
    auto error = [&](const char* key, const char* message) {
        std::fprintf(stderr, "%s: %s %s\n", path.c_str(), key, message);
        ok = false;
    };
    // Removes `key` from `values` and returns its value, if present and of the right kind.
    auto take = [&](const char* key, bool wantArray) -> std::optional<JsonValue> {
        const auto it = values.find(key);
        if (it == values.end()) return std::nullopt;
        const JsonValue value = it->second;
        values.erase(it);
        if (value.isArray != wantArray) {
            error(key, wantArray ? "must be an array of numbers" : "must be a number");
            return std::nullopt;
        }
        return value;
    };

    struct IntSetting {
        const char* key;
        int* target;
    };
    struct FloatSetting {
        const char* key;
        float* target;
    };
    const IntSetting intSettings[] = {
        {"windowWidth", &config.windowWidth},
        {"windowHeight", &config.windowHeight},
        {"targetFps", &config.targetFps},
        {"paddleSections", &config.paddleSections},
    };
    const FloatSetting floatSettings[] = {
        {"ballSpeedX", &config.ballSpeedX},
        {"ballSpeedY", &config.ballSpeedY},
        {"paddleSpeed", &config.paddleSpeed},
        {"ballMaxSpeedMultiplier", &config.ballMaxSpeedMultiplier},
        {"musicBpm", &config.musicBpm},
    };
    // Settings that may be zero, e.g. to turn the speed-up or the music off.
    const FloatSetting nonNegativeSettings[] = {
        {"ballSpeedUpPerHit", &config.ballSpeedUpPerHit},
        {"musicVolume", &config.musicVolume},
    };

    for (const IntSetting& s : intSettings) {
        if (const auto value = take(s.key, false)) {
            if (value->numbers[0] < 1.0) error(s.key, "must be at least 1");
            *s.target = static_cast<int>(value->numbers[0]);
        }
    }
    for (const FloatSetting& s : floatSettings) {
        if (const auto value = take(s.key, false)) {
            if (value->numbers[0] <= 0.0) error(s.key, "must be greater than 0");
            *s.target = static_cast<float>(value->numbers[0]);
        }
    }
    for (const FloatSetting& s : nonNegativeSettings) {
        if (const auto value = take(s.key, false)) {
            if (value->numbers[0] < 0.0) error(s.key, "must not be negative");
            *s.target = static_cast<float>(value->numbers[0]);
        }
    }
    if (const auto value = take("musicMelody", true)) {
        if (value->numbers.empty()) error("musicMelody", "must have at least one note");
        config.musicMelody.clear();
        for (double frequency : value->numbers) {
            if (frequency < 0.0 || frequency > 20000.0) {
                error("musicMelody", "frequencies must be between 0 and 20000 Hz");
                break;
            }
            config.musicMelody.push_back(static_cast<float>(frequency));
        }
    }

    if (config.ballMaxSpeedMultiplier < 1.0f) error("ballMaxSpeedMultiplier", "must be at least 1");
    if (config.musicVolume > 1.0f) error("musicVolume", "must be at most 1");
    for (const auto& [key, value] : values) {
        std::fprintf(stderr, "%s: ignoring unknown setting \"%s\"\n", path.c_str(), key.c_str());
    }
    return ok;
}
