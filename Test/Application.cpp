
#include "Application.h"

Application::Application(): viewport(nullptr)
{
}

Application::~Application()
{
    delete viewport;
}

void Application::init()
{
    viewport = new Viewport("CPU Raster", WINDOW_WIDTH, WINDOW_HEIGHT);
    viewport->init();

    SDL_Surface* texture = SDL_LoadBMP("resources/Testimone.bmp");
    std::cout << SDL_GetError() << '\n';
    
    Camera camera(Vector3f(-0.07f, 0.f, -2), 45.f, 0.1f, 1000.f, Perspective, 10, 10);
    Mesh cube = Mesh::create_cube(.5, {255, 0, 0, 1}, texture);
    Scene scene(camera, {cube});
    viewport->scene = scene; 
}

void Application::run()
{
    while (viewport->get_state() == EViewportState::Running)
    {
        viewport->update();
        viewport->render(viewport->scene);
    }
}
