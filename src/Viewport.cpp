
#include "Viewport.h"
#include <iostream>
#include <SDL3/SDL_events.h>

Viewport::Viewport(const std::string& name, uint16_t width, uint16_t height): surface(nullptr), palette(nullptr),
                                                                              scene({}, {})
{
    this->height = height;
    this->width = width;
    rasterizer = new Rasterizer();
    
    window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_RESIZABLE);
}

Viewport::~Viewport()
{
    SDL_DestroyWindow(window);
    SDL_DestroyPalette(palette);
    SDL_DestroySurface(surface);
    delete rasterizer;
}

void Viewport::init()
{
    surface = SDL_GetWindowSurface(window);
    palette = SDL_CreatePalette(256);
    
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

    // Print any SDL error to the console.
    //std::cout << SDL_GetError() << '\n';

    SDL_Color color{30, 144, 255, 255};
    SDL_SetSurfacePalette(surface, palette);
    const uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format),
                                       palette, color.r, color.g, color.b, color.a);
    SDL_FillSurfaceRect(surface, nullptr, pixel);
    SDL_UpdateWindowSurface(window);
    
    // Initialize viewport matrix diagonal.
    viewport_matrix(0, 0) = width / 2;
    viewport_matrix(1, 1) = height / 2;
    viewport_matrix(2, 2) = 1;
    viewport_matrix(3, 3) = 1;
    
    // Initialize translation components.
    viewport_matrix(0, 3) = (width - 1) / 2;
    viewport_matrix(1, 3) = (height - 1) / 2;
    viewport_matrix(2, 3) = 0;

    this->state = EViewportState::Running;
}

void Viewport::update()
{
    for (SDL_Event event; SDL_PollEvent(&event);)
    {
        switch (event.type)
        {
        default: break;
                
        case SDL_EVENT_WINDOW_RESIZED:
            {
                // Set a new window size.
                width = static_cast<uint16_t>(event.window.data1);
                height = static_cast<uint16_t>(event.window.data2);
                    
                // Init new viewport with updates properties.
                init();
                break;
            }
                
        case SDL_EVENT_QUIT:
            state = EViewportState::Stopped;
            break;
                
        case SDL_EVENT_MOUSE_MOTION:
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            break;
        }
    }
}

void Viewport::render(const Scene& scene)
{
    std::cout << SDL_GetError() << '\n';
    SDL_LockSurface(surface);
    
    const Camera& camera = scene.camera;
    // Draw all the meshes inside the scene.
    for (const Mesh& mesh : scene.meshes)
    {
        Mesh transformed_mesh = mesh;
        
        const Matrix4f& projection_matrix = camera.get_projection_matrix();
        const Matrix4f& view_matrix = camera.get_camera_matrix();

        transformed_mesh.vertices.clear();
        for (const Vertex& vertex : mesh.vertices)
        {
            Matrix<float, 4, 1> vertex_pos;
            vertex_pos(0, 0) = vertex.x;
            vertex_pos(1, 0) = vertex.y;
            vertex_pos(2, 0) = vertex.z;
            vertex_pos(3, 0) = 1.f;

            // Apply transformation to a single vertex position.
            vertex_pos = viewport_matrix * projection_matrix * view_matrix * vertex_pos;
            
            // Homogeneous coordinates conversion.
            if (vertex_pos(3, 0) != 0.f)
            {
                vertex_pos(0, 0) /= vertex_pos(3, 0);
                vertex_pos(1, 0) /= vertex_pos(3, 0);
                vertex_pos(2, 0) /= vertex_pos(3, 0);
                vertex_pos(3, 0) = 1.f;
            }
            
            Vertex transformed_vertex (vertex_pos(0, 0), vertex_pos(1, 0), vertex_pos(2, 0),
                vertex.u, vertex.v, vertex.color);
            transformed_mesh.vertices.push_back(transformed_vertex);
        }

        rasterizer->draw_mesh(surface, palette, transformed_mesh, Wireframe);
    }
    
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
}

EViewportState Viewport::get_state() const
{
    return state;
}
