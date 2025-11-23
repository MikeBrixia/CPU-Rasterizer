
#include <iostream>
#include <SDL3/SDL.h>
#include "Application.h"

void main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Application app;
    app.run();

    SDL_Quit();
}
