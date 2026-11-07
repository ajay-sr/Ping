#include "Ball.h"

#include <algorithm>

Ball::Ball(Vec2 center, float size, float speedX, float speedY, int paddleSections,
           float speedUpPerHit, float maxSpeedMultiplier)
    : center_(center),
      size_(size),
      speedX_(speedX),
      speedY_(speedY),
      paddleSections_(paddleSections),
      speedUpPerHit_(speedUpPerHit),
      maxSpeedMultiplier_(maxSpeedMultiplier) {
    reset();
}

void Ball::reset() {
    position_ = {center_.x - size_ / 2.0f, center_.y - size_ / 2.0f};
    speedMultiplier_ = 1.0f;
    velocity_ = {serveDirection_ * speedX_, 0.0f};
    serveDirection_ = -serveDirection_;
}

void Ball::update(float dt, const Player& left, const Player& right, float boundsHeight) {
    position_.x += velocity_.x * dt;
    position_.y += velocity_.y * dt;

    // Only bounce off the paddle the ball is moving towards, so it can't be flipped twice.
    if (velocity_.x < 0.0f && collidesWith(left)) {
        bounceOffPaddle(left);
    } else if (velocity_.x > 0.0f && collidesWith(right)) {
        bounceOffPaddle(right);
    }

    bounceOffWalls(boundsHeight);
}

bool Ball::collidesWith(const Player& player) const {
    const Vec2& p = player.position();
    const Vec2& s = player.size();
    return position_.x < p.x + s.x && position_.x + size_ > p.x && position_.y < p.y + s.y &&
           position_.y + size_ > p.y;
}

void Ball::bounceOffPaddle(const Player& player) {
    const Vec2& p = player.position();

    // Push the ball out to the paddle's face, then send it back the other way.
    const float direction = velocity_.x < 0.0f ? 1.0f : -1.0f;
    if (direction > 0.0f) {
        position_.x = p.x + player.size().x;
    } else {
        position_.x = p.x - size_;
    }

    // Each hit speeds the ball up a little, until the next point resets it.
    speedMultiplier_ = std::min(speedMultiplier_ * (1.0f + speedUpPerHit_), maxSpeedMultiplier_);
    velocity_.x = direction * speedX_ * speedMultiplier_;

    // Pick the vertical direction from which section of the paddle was hit: -1 for the top
    // section, 1 for the bottom, and evenly spaced values in between.
    const float hitY = position_.y + size_ / 2.0f - p.y;
    const float sectionHeight = player.size().y / paddleSections_;
    const int section =
        std::clamp(static_cast<int>(hitY / sectionHeight), 0, paddleSections_ - 1);
    const float directionY =
        paddleSections_ > 1 ? -1.0f + 2.0f * section / (paddleSections_ - 1) : 0.0f;
    velocity_.y = directionY * speedY_ * speedMultiplier_;
}

void Ball::bounceOffWalls(float boundsHeight) {
    const float maxY = boundsHeight - size_;
    if (position_.y < 0.0f) {
        position_.y = 0.0f;
        velocity_.y = -velocity_.y;
    } else if (position_.y > maxY) {
        position_.y = maxY;
        velocity_.y = -velocity_.y;
    }
}

bool Ball::isPastLeftEdge() const { return position_.x + size_ < 0.0f; }

bool Ball::isPastRightEdge(float boundsWidth) const { return position_.x > boundsWidth; }

void Ball::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const SDL_FRect rect{position_.x, position_.y, size_, size_};
    SDL_RenderFillRectF(renderer, &rect);
}
