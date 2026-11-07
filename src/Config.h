#pragma once

#include <string>
#include <vector>

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

    // Title and game music: a square wave playing one melody entry per beat, looping.
    float musicVolume = 0.25f;  // 0 = silent, 1 = full
    float musicBpm = 60.0f;
    std::vector<float> musicMelody = {  // frequency in Hz per beat; 0 = rest
        440.00f, 523.25f, 659.25f, 587.33f, 523.25f,  // A4 C5 E5 D5 C5
        440.00f, 392.00f, 440.00f, 329.63f, 0.0f,     // A4 G4 A4 E4 rest
    };
};

// Reads settings from a JSON file containing a flat object of numbers and arrays of numbers,
// e.g. {"windowWidth": 800, "musicMelody": [440, 0, 523.25]}. Keys that are absent keep their
// defaults; a missing file leaves every default in place. Returns false, after printing why, if
// the file is malformed or a value is out of range.
bool loadConfig(const std::string& path, Config& config);
