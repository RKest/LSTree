#include "transform.h"
#include "display.h"
#include "camera.h"
#include <iostream>

#include <SDL2/SDL_scancode.h>

class Controler
{
public:
    Controler(Display &display, Camera &camera) : display(display), camera(camera)
    {
        yaw = -90.0f;
        pitch = 0.0f;
    };

    void CaptureMouseMovement();
    void CaptureKeyboardPresses();

protected:
private:
    Display &display;
    Camera &camera;

    int xChange = 0, yChange = 0;
    float yaw, pitch;

    SDL_Scancode *codes = display.KeyScancodeMap();
    float defaultMovementAmount = 0.1f;
};
