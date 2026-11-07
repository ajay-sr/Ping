#include "Game.h"

#include <cstdint>
#include <cstdio>

namespace {

// Layout fractions.
constexpr float kScoreBarFraction = 0.12f;            // of window height
constexpr float kPaddleWidthFraction = 0.025f;        // of field width
constexpr float kPaddleHeightFraction = 1.0f / 6.0f;  // of field height
constexpr float kPaddleMarginFraction = 0.05f;        // of field width
constexpr float kBallSizeFraction = 0.025f;           // of field height

Player makePlayer(const Config& config, float fieldWidth, float fieldHeight, bool left,
                  SDL_Scancode upKey, SDL_Scancode downKey) {
    const Vec2 size{fieldWidth * kPaddleWidthFraction, fieldHeight * kPaddleHeightFraction};
    const float margin = fieldWidth * kPaddleMarginFraction;
    const float x = left ? margin : fieldWidth - size.x - margin;
    const float y = (fieldHeight - size.y) / 2.0f;
    return Player(Vec2{x, y}, size, config.paddleSpeed * fieldHeight, upKey, downKey);
}

}  // namespace

Game::Game(const Config& config)
    : windowWidth_(config.windowWidth),
      windowHeight_(config.windowHeight),
      scoreBarHeight_(windowHeight_ * kScoreBarFraction),
      fieldWidth_(static_cast<float>(windowWidth_)),
      fieldHeight_(windowHeight_ - scoreBarHeight_),
      leftPlayer_(makePlayer(config, fieldWidth_, fieldHeight_, true, SDL_SCANCODE_W,
                             SDL_SCANCODE_S)),
      rightPlayer_(makePlayer(config, fieldWidth_, fieldHeight_, false, SDL_SCANCODE_UP,
                              SDL_SCANCODE_DOWN)),
      ball_(Vec2{fieldWidth_ / 2.0f, fieldHeight_ / 2.0f}, fieldHeight_ * kBallSizeFraction,
            config.ballSpeedX * fieldWidth_, config.ballSpeedY * fieldHeight_,
            config.paddleSections, config.ballSpeedUpPerHit, config.ballMaxSpeedMultiplier),
      jukebox_(config.musicMelody, config.musicBpm, config.musicVolume) {}

Game::~Game() {
    jukebox_.close();  // before SDL_Quit shuts the audio subsystem down
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow("Ping", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               windowWidth_, windowHeight_, SDL_WINDOW_SHOWN);
    if (!window_) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }

    // Music is optional: without audio the game runs silently.
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        std::fprintf(stderr, "SDL audio init failed: %s; continuing without music\n",
                     SDL_GetError());
    } else if (!jukebox_.open()) {
        std::fprintf(stderr, "Continuing without music\n");
    }

    return true;
}

void Game::run() {
    const std::uint64_t perfFreq = SDL_GetPerformanceFrequency();
    const std::uint64_t ticksPerFrame = perfFreq / kTargetFps;
    const float dt = 1.0f / kTargetFps;

    // The music starts on the title screen and keeps looping through the game.
    jukebox_.play();

    running_ = true;
    while (running_) {
        const std::uint64_t frameStart = SDL_GetPerformanceCounter();

        processEvents();
        update(dt);
        render();

        // Frame cap: sleep for most of the remaining time, then spin for precision.
        const std::uint64_t frameEnd = frameStart + ticksPerFrame;
        const std::uint64_t now = SDL_GetPerformanceCounter();
        if (now < frameEnd) {
            const std::uint64_t remainingMs = (frameEnd - now) * 1000 / perfFreq;
            if (remainingMs > 1) {
                SDL_Delay(static_cast<Uint32>(remainingMs - 1));
            }
            while (SDL_GetPerformanceCounter() < frameEnd) {
            }
        }
    }
}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running_ = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            running_ = false;
        } else if (event.type == SDL_KEYDOWN && !event.key.repeat && screen_ == Screen::Title) {
            screen_ = Screen::Playing;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    leftPlayer_.handleInput(keys);
    rightPlayer_.handleInput(keys);
}

void Game::update(float dt) {
    if (screen_ != Screen::Playing) return;

    leftPlayer_.update(dt, fieldHeight_);
    rightPlayer_.update(dt, fieldHeight_);

    ball_.update(dt, leftPlayer_, rightPlayer_, fieldHeight_);
    // A ball past the left edge is a point for player 2, past the right edge for player 1.
    if (ball_.isPastLeftEdge()) {
        scoreBoard_.increment(2);
        ball_.reset();
    } else if (ball_.isPastRightEdge(fieldWidth_)) {
        scoreBoard_.increment(1);
        ball_.reset();
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    SDL_RenderSetViewport(renderer_, nullptr);

    if (screen_ == Screen::Title) {
        titleScreen_.render(renderer_, static_cast<float>(windowWidth_),
                            static_cast<float>(windowHeight_));
        SDL_RenderPresent(renderer_);
        return;
    }

    scoreBoard_.render(renderer_, fieldWidth_, scoreBarHeight_);

    // Draw the play field below the score bar so game coordinates stay relative to it.
    const int fieldTop = static_cast<int>(scoreBarHeight_);
    const SDL_Rect field{0, fieldTop, windowWidth_, windowHeight_ - fieldTop};
    SDL_RenderSetViewport(renderer_, &field);
    leftPlayer_.render(renderer_);
    rightPlayer_.render(renderer_);
    ball_.render(renderer_);

    SDL_RenderPresent(renderer_);
}
