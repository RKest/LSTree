#include <GL/glew.h>
#include <iostream>
#include "display.h"

Display::Display(int _width, int _height, const std::string &title)
{
    width = _width;
    height = _height;
    aspectRatio = static_cast<float>(_width) / static_cast<float>(_height);

    halfWidth = static_cast<float>(_width) / 2;
    halfHeight = static_cast<float>(_height) / 2;

    std::cout << "Constructed" << std::endl;
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_CUBE_MAP);

    GLenum status = glewInit();

    if (status != GLEW_OK)
    {
        std::cerr << "Glew failed to initialise" << std::endl;
    }

    isClosed = false;
}

Display::~Display()
{
    std::cout << "Destructed" << std::endl;
    // TTF_CloseFont(font);
    // SDL_DestroyRenderer(renderer);

    // font = NULL;
    // renderer = NULL;

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::Update()
{
    SDL_GL_SwapWindow(window);
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            isClosed = true;
    }
}

bool Display::IsClosed()
{
    return isClosed;
}

void Display::Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float Display::Aspect()
{
    return aspectRatio;
}

void Display::FetchMouseCoordsNormalised(int &x, int &y)
{
    SDL_GetRelativeMouseState(&x, &y);
}

bool Display::ReadKeyboardState(SDL_Scancode code)
{
    return keys[code];
}

SDL_Scancode *Display::KeyScancodeMap()
{
    return keyScancodeMap;
}

// TTF_Font *Display::LoadFont(const std::string &path)
// {
//     TTF_Font *font = TTF_OpenFont(path.c_str(), 24);
//     if (!font)
//     {
//         std::cerr << "Error: " << TTF_GetError() << std::endl;
//     }

//     return font;
// }

// void Display::RenderText(const std::string &text, SDL_Rect textBox)
// {
//     font = LoadFont("./res/Roboto-Black.ttf");
//     renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     SDL_Color fg = {0, 0, 0};
//     SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), fg);

//     textBox.w = surf->w;
//     textBox.h = surf->h;

//     SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

//     SDL_RenderCopy(renderer, tex, NULL, &textBox);
//     SDL_DestroyTexture(tex);
//     SDL_FreeSurface(surf);
// }