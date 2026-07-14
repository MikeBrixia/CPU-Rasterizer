
#include "Application.h"

Application::Application()
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
}

void Application::run()
{
    while (viewport->get_state() == EViewportState::Running)
    {
        viewport->update();
        viewport->render();
    }
}
