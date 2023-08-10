#include "camera.h"
#include "transform.h"

#include <functional>
#include <iostream>

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>

class Controller
{
public:
    Controller(Camera& camera);

    enum Key
    {
        W,
        A,
        S,
        D,
        LCTRL,
        SPACE
    };

    void invokeMouseEvents();
    void invokeKeyboardEvents();
    void bindTo(Key key, std::function<void()> callback);
    void bindToFront(Key key, auto&& function, auto&&... args)
    {
        bindTo(key, std::bind_front(std::forward<decltype(function)>(function),
                                    std::forward<decltype(args)>(args)...));
    }

private:
    void moveForward();
    void moveBackwards();
    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();

    Camera& camera;

    int xChange{0};
    int yChange{0};
    float yaw{-90.F};
    float pitch{0.F};

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    SDL_Scancode key_scancode_map[6] = {SDL_SCANCODE_W, SDL_SCANCODE_A,     SDL_SCANCODE_S,
                                        SDL_SCANCODE_D, SDL_SCANCODE_LCTRL, SDL_SCANCODE_SPACE};
    struct KeyMap
    {
        Key key;
        std::function<void()> callback;
    };
    std::vector<KeyMap> key_callbacks;

    constexpr static float default_stride = 0.1f;
};
