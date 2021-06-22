#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <SDL2/SDL.h>
// #include <SDL_ttf.h>

class Display
{
public:
    Display(int _width, int _height, const std::string &title);

    void Clear(float r, float g, float b, float a);
    void Update();
    void FetchMouseCoordsNormalised(int &x, int &y);
    bool ReadKeyboardState(SDL_Scancode code);

    // void RenderText(const std::string &text, SDL_Rect textBox);
    // TTF_Font* LoadFont(const std::string &path);

    enum { W, A, S, D, LCTRL, SPACE};
    SDL_Scancode *KeyScancodeMap();

    bool IsClosed();
    float Aspect();
    virtual ~Display();

protected:
private:
    SDL_Window *window;
    SDL_GLContext glContext;
    bool isClosed;
    float aspectRatio;
    int width, height;
    float halfWidth, halfHeight;
    int mouseX = 0, mouseY = 0;

    // TTF_Font *font;
    // SDL_Renderer* renderer;

    //Offset used to keep track of the position after the mouse wrapping from CenterMouseInWindow();
    int mouseXOffset = 0, mouseYOffset = 0;
    bool mouseEventFilter = false;

    SDL_Scancode keyScancodeMap[6] = { SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_LCTRL, SDL_SCANCODE_SPACE };

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
};

#endif // DISPLAY_H
