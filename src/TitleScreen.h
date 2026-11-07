#pragma once

#include <SDL2/SDL.h>

class TitleScreen {
public:
    // Draws the title and the "press any key" prompt on a `width` x `height` black screen.
    void render(SDL_Renderer* renderer, float width, float height) const;
};
