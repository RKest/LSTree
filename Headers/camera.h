#ifndef CAMERA_H
#define CAMERA_H

#include "_config.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
    Camera(const glm::vec3 &pos, f fov, f aspect, f zNear, f zFar)
    {
        perspective = glm::perspective(fov, aspect, zNear, zFar);
        position = pos;
        forward = glm::vec3(0, 0, 1);
        up = glm::vec3(0, 1, 0);
    }

    inline glm::vec3 &Pos() { return position; }
    inline glm::vec3 &Fwd() { return forward; }
    inline glm::vec3 &Up() { return up; }

    inline glm::mat4 ViewProjection() const
    {
        return perspective * glm::lookAt(position, position + forward, up);
    }

protected:
private:
    glm::mat4 perspective;
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
};

#endif //CAMERA_H