#include "Config.h"
#include "Game.h"

int main(int argc, char* argv[]) {
    // Settings come from the file given as the first argument, or config.json in the
    // current directory.
    Config config;
    if (!loadConfig(argc > 1 ? argv[1] : "config.json", config)) {
        return 1;
    }

    Game game(config);
    if (!game.init()) {
        return 1;
    }
    game.run();
    return 0;
}
