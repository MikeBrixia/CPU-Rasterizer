
#include <iostream>
#include <SDL3/SDL.h>
#include "src/Application.h"

void main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    Application app;
    app.run();

    SDL_Quit();
}
