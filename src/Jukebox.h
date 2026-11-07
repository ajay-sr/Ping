#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <vector>

// Plays a looping melody as a single square-wave channel.
class Jukebox {
public:
    // `melody` holds one frequency in Hz per beat (0 = rest), played at `bpm` beats per minute
    // and looped. `volume` runs from 0 (silent) to 1 (full).
    Jukebox(std::vector<float> melody, float bpm, float volume);
    ~Jukebox();

    Jukebox(const Jukebox&) = delete;
    Jukebox& operator=(const Jukebox&) = delete;

    // Opens the audio device; call after SDL_Init(SDL_INIT_AUDIO). Returns false, after
    // printing why, if no audio device is available.
    bool open();
    void close();

    void play();
    void stop();

private:
    static void audioCallback(void* userdata, Uint8* stream, int length);
    void fill(float* samples, int count);

    std::vector<float> melody_;
    float bpm_;
    float volume_;

    SDL_AudioDeviceID device_ = 0;
    int sampleRate_ = 0;

    // Playback position, only touched on the audio thread once the device is open.
    std::uint64_t samplePosition_ = 0;
    float phase_ = 0.0f;  // position within the current square wave cycle, 0 to 1
};
