#include "Config.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>

namespace {

// Minimal parser for a flat JSON object whose values are all numbers.
class FlatJsonParser {
public:
    explicit FlatJsonParser(std::string text) : text_(std::move(text)) {}

    bool parse(std::map<std::string, double>& out) {
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
            double value = 0.0;
            if (!parseNumber(value)) return false;
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

    std::map<std::string, double> values;
    FlatJsonParser parser(buffer.str());
    if (!parser.parse(values)) {
        std::fprintf(stderr, "%s: %s\n", path.c_str(), parser.error().c_str());
        return false;
    }

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
        {"paddleSections", &config.paddleSections},
    };
    const FloatSetting floatSettings[] = {
        {"ballSpeedX", &config.ballSpeedX},
        {"ballSpeedY", &config.ballSpeedY},
        {"paddleSpeed", &config.paddleSpeed},
        {"ballMaxSpeedMultiplier", &config.ballMaxSpeedMultiplier},
    };
    // Settings that may be zero, e.g. to turn the speed-up off.
    const FloatSetting nonNegativeSettings[] = {
        {"ballSpeedUpPerHit", &config.ballSpeedUpPerHit},
    };

    bool ok = true;
    for (const IntSetting& s : intSettings) {
        const auto it = values.find(s.key);
        if (it == values.end()) continue;
        if (it->second < 1.0) {
            std::fprintf(stderr, "%s: %s must be at least 1\n", path.c_str(), s.key);
            ok = false;
        }
        *s.target = static_cast<int>(it->second);
        values.erase(it);
    }
    for (const FloatSetting& s : floatSettings) {
        const auto it = values.find(s.key);
        if (it == values.end()) continue;
        if (it->second <= 0.0) {
            std::fprintf(stderr, "%s: %s must be greater than 0\n", path.c_str(), s.key);
            ok = false;
        }
        *s.target = static_cast<float>(it->second);
        values.erase(it);
    }
    for (const FloatSetting& s : nonNegativeSettings) {
        const auto it = values.find(s.key);
        if (it == values.end()) continue;
        if (it->second < 0.0) {
            std::fprintf(stderr, "%s: %s must not be negative\n", path.c_str(), s.key);
            ok = false;
        }
        *s.target = static_cast<float>(it->second);
        values.erase(it);
    }
    if (config.ballMaxSpeedMultiplier < 1.0f) {
        std::fprintf(stderr, "%s: ballMaxSpeedMultiplier must be at least 1\n", path.c_str());
        ok = false;
    }
    for (const auto& [key, value] : values) {
        std::fprintf(stderr, "%s: ignoring unknown setting \"%s\"\n", path.c_str(), key.c_str());
    }
    return ok;
}
