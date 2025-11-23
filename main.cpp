
#include <iostream>
#include <SDL3/SDL.h>
#include "Test/Application.h"

#define ENABLE_TEST_APPLICATION 1

void main(int argc, char* argv[])
{
    // Initialize SDL library.
    SDL_Init(SDL_INIT_VIDEO);

#if ENABLE_TEST_APPLICATION
    Application app;
    app.run();
#endif

    // Terminate SDL library.
    SDL_Quit();
}
