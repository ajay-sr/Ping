#pragma once

#include <SDL2/SDL.h>

#include "Ball.h"
#include "Config.h"
#include "Jukebox.h"
#include "Player.h"
#include "ScoreBoard.h"
#include "TitleScreen.h"

class Game {
public:
    static constexpr int kTargetFps = 60;

    explicit Game(const Config& config);
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool init();
    void run();

private:
    enum class Screen { Title, Playing };

    void processEvents();
    void update(float dt);
    void render();

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    bool running_ = false;
    Screen screen_ = Screen::Title;

    // Layout in pixels, derived from the config's window size.
    int windowWidth_;
    int windowHeight_;
    float scoreBarHeight_;
    float fieldWidth_;
    float fieldHeight_;

    Player leftPlayer_;
    Player rightPlayer_;
    Ball ball_;
    ScoreBoard scoreBoard_;
    TitleScreen titleScreen_;
    Jukebox jukebox_;
};
