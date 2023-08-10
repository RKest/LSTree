#include "controller.h"

#include <iostream>

#include <SDL2/SDL.h>

Controller::Controller(Camera& new_camera) : camera{new_camera}
{
    bindToFront(W, &Controller::moveForward, this);
    bindToFront(S, &Controller::moveBackwards, this);
    bindToFront(A, &Controller::moveLeft, this);
    bindToFront(D, &Controller::moveRight, this);
    bindToFront(LCTRL, &Controller::moveDown, this);
    bindToFront(SPACE, &Controller::moveUp, this);
}
void Controller::moveForward()
{
    camera.Pos() += default_stride * glm::vec3(camera.Fwd().x, 0, camera.Fwd().z);
}
void Controller::moveBackwards()
{
    camera.Pos() -= default_stride * glm::vec3(camera.Fwd().x, 0, camera.Fwd().z);
}
void Controller::moveLeft()
{
    camera.Pos() -= default_stride * glm::normalize(glm::cross(camera.Fwd(), camera.Up()));
}
void Controller::moveRight()
{
    camera.Pos() += default_stride * glm::normalize(glm::cross(camera.Fwd(), camera.Up()));
}
void Controller::moveDown()
{
    camera.Pos() -= default_stride * glm::vec3(0, 1, 0);
}
void Controller::moveUp()
{
    camera.Pos() += default_stride * glm::vec3(0, 1, 0);
}
void Controller::bindTo(Controller::Key key, std::function<void()> callback)
{
    key_callbacks.emplace_back(key, std::move(callback));
}

void Controller::invokeMouseEvents()
{
    SDL_GetRelativeMouseState(&xChange, &yChange);

    float sens = 0.1f;
    yaw += xChange * sens;
    pitch -= yChange * sens;
    pitch = std::clamp(pitch, -89.F, 89.F);

    camera.Fwd().x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.Fwd().y = sin(glm::radians(pitch));
    camera.Fwd().z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.Fwd() = glm::normalize(camera.Fwd());
}

void Controller::invokeKeyboardEvents()
{
    for (auto& [key, callback] : key_callbacks)
    {
        if (keys[key_scancode_map[key]])
        {
            callback();
        }
    }
}
