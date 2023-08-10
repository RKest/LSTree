#include "display.h"
#include <GL/glew.h>
#include <iostream>

Display::Display(int width, int height, const std::string& title) :
    aspect_ratio{static_cast<float>(width) / static_cast<float>(height)}
{
    std::cout << "Constructed" << std::endl;
    initSDL(width, height, title);
    initGLOptions();
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Glew failed to initialise" << std::endl;
    }
}

void Display::initSDL(int width, int height, const std::string& title)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
                              height, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Display::initGLOptions()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_CUBE_MAP);
}

Display::~Display()
{
    std::cout << "Destructed" << std::endl;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::update()
{
    SDL_GL_SwapWindow(window);
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            is_closed = true;
    }
}

auto Display::isClosed() -> bool const&
{
    return is_closed;
}

void Display::clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

auto Display::aspectRatio() -> float const&
{
    return aspect_ratio;
}

void Display::close()
{
    is_closed = true;
}

void Display::fetchMouseCoordsNormalised(int& x, int& y)
{

}