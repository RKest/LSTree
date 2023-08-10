#include "controler.h"
#include <iostream>

void Controler::CaptureMouseMovement()
{
    display.FetchMouseCoordsNormalised(xChange, yChange);

    float sens = 0.1f;
    yaw += xChange * sens;
    pitch -= yChange * sens;
    pitch = std::clamp(pitch, -89.F, 89.F);

    camera.Fwd().x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.Fwd().y = sin(glm::radians(pitch));
    camera.Fwd().z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.Fwd() = glm::normalize(camera.Fwd());
}

void Controler::CaptureKeyboardPresses()
{
    if (display.ReadKeyboardState(codes[display.W]))
        camera.Pos() += defaultMovementAmount * glm::vec3(camera.Fwd().x, 0, camera.Fwd().z);
    if (display.ReadKeyboardState(codes[display.S]))
        camera.Pos() -= defaultMovementAmount * glm::vec3(camera.Fwd().x, 0, camera.Fwd().z);
    if (display.ReadKeyboardState(codes[display.A]))
        camera.Pos() -=
            defaultMovementAmount * glm::normalize(glm::cross(camera.Fwd(), camera.Up()));
    if (display.ReadKeyboardState(codes[display.D]))
        camera.Pos() +=
            defaultMovementAmount * glm::normalize(glm::cross(camera.Fwd(), camera.Up()));
    if (display.ReadKeyboardState(codes[display.LCTRL]))
        camera.Pos() -= defaultMovementAmount * glm::vec3(0, 1, 0);
    if (display.ReadKeyboardState(codes[display.SPACE]))
        camera.Pos() += defaultMovementAmount * glm::vec3(0, 1, 0);
}
