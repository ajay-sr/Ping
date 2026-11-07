#pragma once

#include <SDL2/SDL.h>

#include "Player.h"
#include "Vec2.h"

class Ball {
public:
    // `size` is the side length of the square ball. Speeds are in pixels per second;
    // `speedY` is the vertical speed after a hit on the very top or bottom of a paddle.
    // Paddles are split into `paddleSections` equal sections whose vertical directions are
    // spaced evenly from -1 (top) to 1 (bottom). Each paddle hit multiplies the ball's speed by
    // (1 + `speedUpPerHit`), up to `maxSpeedMultiplier`.
    Ball(Vec2 center, float size, float speedX, float speedY, int paddleSections,
         float speedUpPerHit, float maxSpeedMultiplier);

    // Moves the ball and resolves collisions with both paddles and the top/bottom walls.
    void update(float dt, const Player& left, const Player& right, float boundsHeight);
    void render(SDL_Renderer* renderer) const;

    // True once the ball has gone fully past the left or right edge of the screen.
    bool isPastLeftEdge() const;
    bool isPastRightEdge(float boundsWidth) const;

    // Returns the ball to the center at its base speed and serves it in the opposite direction
    // to the last serve.
    void reset();

private:
    bool collidesWith(const Player& player) const;
    void bounceOffPaddle(const Player& player);
    void bounceOffWalls(float boundsHeight);

    Vec2 center_;
    float size_;
    float speedX_;
    float speedY_;
    int paddleSections_;
    float speedUpPerHit_;
    float maxSpeedMultiplier_;
    float speedMultiplier_ = 1.0f;  // grows with each paddle hit in a rally
    Vec2 position_;  // top-left corner
    Vec2 velocity_;
    float serveDirection_ = -1.0f;
};
