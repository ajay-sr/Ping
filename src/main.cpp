#include <SDL2/SDL.h>

#include <fstream>
#include <string>

#include "Config.h"
#include "Game.h"

namespace {

// The config file given as the first argument; otherwise config.json in the current
// directory, or next to the executable (where release archives put it).
std::string findConfigPath(int argc, char* argv[]) {
    if (argc > 1) return argv[1];

    const std::string fileName = "config.json";
    if (std::ifstream(fileName)) return fileName;

    if (char* basePath = SDL_GetBasePath()) {
        const std::string besideExecutable = basePath + fileName;
        SDL_free(basePath);
        if (std::ifstream(besideExecutable)) return besideExecutable;
    }
    return fileName;
}

}  // namespace

int main(int argc, char* argv[]) {
    Config config;
    if (!loadConfig(findConfigPath(argc, argv), config)) {
        return 1;
    }

    Game game(config);
    if (!game.init()) {
        return 1;
    }
    game.run();
    return 0;
}
