#pragma once

#include <SDL2/SDL.h>

class ScoreBoard {
public:
    // Adds a point for player 1 (left) or player 2 (right); other values are ignored.
    void increment(int player);
    void reset();

    int score(int player) const;

    // Draws both scores in a `width` x `height` strip at the top of the current render target.
    // Digit sizes scale with `height`.
    void render(SDL_Renderer* renderer, float width, float height) const;

private:
    int scores_[2] = {0, 0};
};
