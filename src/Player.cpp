#include "Player.h"

Player::Player(Vec2 startPosition, Vec2 size, float speed, SDL_Scancode upKey,
               SDL_Scancode downKey)
    : position_(startPosition), size_(size), speed_(speed), upKey_(upKey), downKey_(downKey) {}

void Player::handleInput(const Uint8* keyboardState) {
    direction_ = 0.0f;
    if (keyboardState[upKey_]) direction_ -= 1.0f;
    if (keyboardState[downKey_]) direction_ += 1.0f;
}

void Player::update(float dt, float boundsHeight) {
    position_.y += direction_ * speed_ * dt;

    const float maxY = boundsHeight - size_.y;
    if (position_.y < 0.0f) position_.y = 0.0f;
    if (position_.y > maxY) position_.y = maxY;
}

void Player::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const SDL_FRect rect{position_.x, position_.y, size_.x, size_.y};
    SDL_RenderFillRectF(renderer, &rect);
}
