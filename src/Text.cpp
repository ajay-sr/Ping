#include "Text.h"

#include <cctype>
#include <cstdint>

namespace {

// --- Seven-segment font ---

// Glyph proportions as fractions of the glyph height.
constexpr float kSegmentWidthFraction = 0.6f;
constexpr float kSegmentStrokeFraction = 0.12f;
constexpr float kSegmentGapFraction = 0.2f;

// Seven-segment bit masks, bits a-g = 0-6:
//  aaa
// f   b
//  ggg
// e   c
//  ddd
constexpr std::uint8_t kDigitSegments[10] = {
    0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110,
    0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111,
};

std::uint8_t segmentsFor(char c) {
    if (c >= '0' && c <= '9') return kDigitSegments[c - '0'];
    switch (std::toupper(static_cast<unsigned char>(c))) {
        case 'P': return 0b1110011;
        case 'I': return 0b0000110;  // drawn one stroke wide, see segmentGlyphWidth()
        case 'N': return 0b0110111;
        case 'G': return 0b0111101;
        default: return 0;
    }
}

// 'I' is a single vertical stroke, so it only takes up a stroke's width.
float segmentGlyphWidth(char c, float width, float stroke) {
    return std::toupper(static_cast<unsigned char>(c)) == 'I' ? stroke : width;
}

void drawSegments(SDL_Renderer* renderer, std::uint8_t segments, float x, float y, float w,
                  float h, float s) {
    const float mid = y + (h - s) / 2.0f;
    const SDL_FRect rects[7] = {
        {x, y, w, s},                            // a
        {x + w - s, y, s, h / 2.0f},             // b
        {x + w - s, y + h / 2.0f, s, h / 2.0f},  // c
        {x, y + h - s, w, s},                    // d
        {x, y + h / 2.0f, s, h / 2.0f},          // e
        {x, y, s, h / 2.0f},                     // f
        {x, mid, w, s},                          // g
    };
    for (int i = 0; i < 7; ++i) {
        if (segments & (1 << i)) {
            SDL_RenderFillRectF(renderer, &rects[i]);
        }
    }
}

// --- 5x7 pixel font ---

constexpr int kPixelColumns = 5;
constexpr int kPixelRows = 7;
constexpr int kPixelSpacing = 1;  // blank columns between glyphs

using PixelGlyph = std::uint8_t[kPixelRows];  // one row per byte, leftmost pixel = bit 4

constexpr PixelGlyph kPixelLetters[26] = {
    {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001},  // A
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110},  // B
    {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110},  // C
    {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110},  // D
    {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111},  // E
    {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000},  // F
    {0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111},  // G
    {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001},  // H
    {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},  // I
    {0b00111, 0b00010, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100},  // J
    {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001},  // K
    {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111},  // L
    {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001},  // M
    {0b10001, 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001},  // N
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},  // O
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000},  // P
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101},  // Q
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001},  // R
    {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110},  // S
    {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100},  // T
    {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},  // U
    {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100},  // V
    {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010},  // W
    {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001},  // X
    {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100},  // Y
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111},  // Z
};

constexpr PixelGlyph kPixelDigits[10] = {
    {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110},  // 0
    {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},  // 1
    {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111},  // 2
    {0b11111, 0b00010, 0b00100, 0b00010, 0b00001, 0b10001, 0b01110},  // 3
    {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010},  // 4
    {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110},  // 5
    {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110},  // 6
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000},  // 7
    {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110},  // 8
    {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100},  // 9
};

// Returns the glyph for `c`, or nullptr for characters drawn as spaces.
const std::uint8_t* pixelGlyphFor(char c) {
    if (c >= '0' && c <= '9') return kPixelDigits[c - '0'];
    const int upper = std::toupper(static_cast<unsigned char>(c));
    if (upper >= 'A' && upper <= 'Z') return kPixelLetters[upper - 'A'];
    return nullptr;
}

}  // namespace

void drawSegmentText(SDL_Renderer* renderer, const std::string& text, float centerX, float y,
                     float height) {
    const float width = height * kSegmentWidthFraction;
    const float stroke = height * kSegmentStrokeFraction;
    const float gap = height * kSegmentGapFraction;

    float totalWidth = 0.0f;
    for (char c : text) {
        totalWidth += segmentGlyphWidth(c, width, stroke) + gap;
    }
    if (!text.empty()) totalWidth -= gap;

    float x = centerX - totalWidth / 2.0f;
    for (char c : text) {
        const float glyphWidth = segmentGlyphWidth(c, width, stroke);
        drawSegments(renderer, segmentsFor(c), x, y, glyphWidth, height, stroke);
        x += glyphWidth + gap;
    }
}

void drawPixelText(SDL_Renderer* renderer, const std::string& text, float centerX, float y,
                   float pixelSize) {
    const float advance = (kPixelColumns + kPixelSpacing) * pixelSize;
    const float totalWidth =
        text.empty() ? 0.0f : text.size() * advance - kPixelSpacing * pixelSize;

    float x = centerX - totalWidth / 2.0f;
    for (char c : text) {
        if (const std::uint8_t* glyph = pixelGlyphFor(c)) {
            for (int row = 0; row < kPixelRows; ++row) {
                for (int col = 0; col < kPixelColumns; ++col) {
                    if (glyph[row] & (1 << (kPixelColumns - 1 - col))) {
                        const SDL_FRect pixel{x + col * pixelSize, y + row * pixelSize,
                                              pixelSize, pixelSize};
                        SDL_RenderFillRectF(renderer, &pixel);
                    }
                }
            }
        }
        x += advance;
    }
}
