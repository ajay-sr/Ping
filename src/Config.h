#pragma once

#include <string>

// Game settings, loaded at startup. Every size in the game is derived from the window size,
// and every speed is a fraction of the play field size.
struct Config {
    // Window size in pixels, including the score bar above the play field.
    int windowWidth = 800;
    int windowHeight = 680;

    // Speeds as fractions of the play field per second.
    float ballSpeedX = 0.375f;  // of field width
    float ballSpeedY = 0.84f;   // of field height, after a hit on a paddle's top or bottom edge
    float paddleSpeed = 0.67f;  // of field height

    // Paddles are split into this many equal sections; the ball's vertical direction after a
    // hit is spaced evenly from -1 (top section) to 1 (bottom section).
    int paddleSections = 9;

    // Each paddle hit multiplies the ball's speed by (1 + ballSpeedUpPerHit), capped at
    // ballMaxSpeedMultiplier times its base speed. A point resets it to base speed.
    float ballSpeedUpPerHit = 0.05f;
    float ballMaxSpeedMultiplier = 3.0f;
};

// Reads settings from a JSON file containing a flat object of numbers, e.g.
// {"windowWidth": 800, "ballSpeedX": 0.375}. Keys that are absent keep their defaults; a
// missing file leaves every default in place. Returns false, after printing why, if the file
// is malformed or a value is out of range.
bool loadConfig(const std::string& path, Config& config);
