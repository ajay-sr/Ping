#include "TitleScreen.h"

#include "Text.h"

namespace {

// Sizes and positions as fractions of the window height.
constexpr float kTitleHeightFraction = 0.2f;
constexpr float kTitleCenterFraction = 0.42f;
constexpr float kSubtitlePixelFraction = 0.006f;  // size of one font pixel
constexpr float kSubtitleTopFraction = 0.65f;

}  // namespace

void TitleScreen::render(SDL_Renderer* renderer, float width, float height) const {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    const float titleHeight = height * kTitleHeightFraction;
    drawSegmentText(renderer, "PING", width / 2.0f,
                    height * kTitleCenterFraction - titleHeight / 2.0f, titleHeight);
    drawPixelText(renderer, "Press any key to start", width / 2.0f, height * kSubtitleTopFraction,
                  height * kSubtitlePixelFraction);
}
