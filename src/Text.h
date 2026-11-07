#pragma once

#include <SDL2/SDL.h>

#include <string>

// Text drawn from filled rectangles in the renderer's current draw color.

// Seven-segment text, as used by the score. Supports digits, spaces and the letters P, I, N
// and G; other characters are drawn as spaces. `text` is centered horizontally on `centerX`,
// with its top at `y`, and each glyph is `height` tall.
void drawSegmentText(SDL_Renderer* renderer, const std::string& text, float centerX, float y,
                     float height);

// 5x7 pixel text. Supports letters (drawn as capitals), digits and spaces; other characters
// are drawn as spaces. `text` is centered horizontally on `centerX`, with its top at `y`, and
// each font pixel is `pixelSize` square.
void drawPixelText(SDL_Renderer* renderer, const std::string& text, float centerX, float y,
                   float pixelSize);
