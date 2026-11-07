#include "ScoreBoard.h"

#include <string>

#include "Text.h"

namespace {

// Sizes as fractions of the score bar height.
constexpr float kDigitHeightFraction = 0.625f;
constexpr float kDividerFraction = 0.025f;

}  // namespace

void ScoreBoard::increment(int player) {
    if (player == 1 || player == 2) {
        ++scores_[player - 1];
    }
}

void ScoreBoard::reset() {
    scores_[0] = 0;
    scores_[1] = 0;
}

int ScoreBoard::score(int player) const {
    return (player == 1 || player == 2) ? scores_[player - 1] : 0;
}

void ScoreBoard::render(SDL_Renderer* renderer, float width, float height) const {
    // Divider line between the score strip and the play field.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const float dividerHeight = height * kDividerFraction;
    const SDL_FRect divider{0.0f, height - dividerHeight, width, dividerHeight};
    SDL_RenderFillRectF(renderer, &divider);

    const float digitHeight = height * kDigitHeightFraction;
    const float y = (height - digitHeight) / 2.0f;
    drawSegmentText(renderer, std::to_string(scores_[0]), width / 4.0f, y, digitHeight);
    drawSegmentText(renderer, std::to_string(scores_[1]), width * 3.0f / 4.0f, y, digitHeight);
}
