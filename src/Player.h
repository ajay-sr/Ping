#pragma once

#include <SDL2/SDL.h>

#include "Vec2.h"

class Player {
public:
    // `size` is the paddle's width and height; `speed` is in pixels per second.
    Player(Vec2 startPosition, Vec2 size, float speed, SDL_Scancode upKey, SDL_Scancode downKey);

    void handleInput(const Uint8* keyboardState);
    void update(float dt, float boundsHeight);
    void render(SDL_Renderer* renderer) const;

    const Vec2& position() const { return position_; }
    const Vec2& size() const { return size_; }

private:
    Vec2 position_;
    Vec2 size_;
    float speed_;
    SDL_Scancode upKey_;
    SDL_Scancode downKey_;
    float direction_ = 0.0f;  // -1 up, +1 down, 0 idle
};
