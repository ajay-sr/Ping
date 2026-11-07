#include "Jukebox.h"

#include <cstdio>
#include <utility>

namespace {

constexpr int kSampleRate = 44100;
constexpr Uint16 kBufferSamples = 1024;
constexpr float kNoteLengthFraction = 0.9f;  // of a beat; the rest is silence between notes

}  // namespace

Jukebox::Jukebox(std::vector<float> melody, float bpm, float volume)
    : melody_(std::move(melody)), bpm_(bpm), volume_(volume) {}

Jukebox::~Jukebox() { close(); }

bool Jukebox::open() {
    SDL_AudioSpec desired{};
    desired.freq = kSampleRate;
    desired.format = AUDIO_F32SYS;
    desired.channels = 1;
    desired.samples = kBufferSamples;
    desired.callback = &Jukebox::audioCallback;
    desired.userdata = this;

    SDL_AudioSpec obtained{};
    device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained,
                                  SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (device_ == 0) {
        std::fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return false;
    }
    sampleRate_ = obtained.freq;
    return true;
}

void Jukebox::close() {
    if (device_ != 0) {
        SDL_CloseAudioDevice(device_);
        device_ = 0;
    }
}

void Jukebox::play() {
    if (device_ != 0) SDL_PauseAudioDevice(device_, 0);
}

void Jukebox::stop() {
    if (device_ != 0) SDL_PauseAudioDevice(device_, 1);
}

void Jukebox::audioCallback(void* userdata, Uint8* stream, int length) {
    static_cast<Jukebox*>(userdata)->fill(reinterpret_cast<float*>(stream),
                                          length / static_cast<int>(sizeof(float)));
}

void Jukebox::fill(float* samples, int count) {
    const auto samplesPerBeat = static_cast<std::uint64_t>(sampleRate_ * 60.0f / bpm_);
    const auto noteSamples = static_cast<std::uint64_t>(samplesPerBeat * kNoteLengthFraction);

    for (int i = 0; i < count; ++i) {
        const std::uint64_t beat = samplePosition_ / samplesPerBeat;
        const std::uint64_t positionInBeat = samplePosition_ % samplesPerBeat;
        const float frequency = melody_.empty() ? 0.0f : melody_[beat % melody_.size()];

        if (frequency > 0.0f && positionInBeat < noteSamples) {
            samples[i] = phase_ < 0.5f ? volume_ : -volume_;
            phase_ += frequency / sampleRate_;
            if (phase_ >= 1.0f) phase_ -= 1.0f;
        } else {
            samples[i] = 0.0f;
            phase_ = 0.0f;
        }
        ++samplePosition_;
    }
}
