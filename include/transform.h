#ifndef MY_TRANSFORM_H
#define MY_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
    Transform(const glm::vec3 &pos = glm::vec3(), const glm::vec3 &rot = glm::vec3(), const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f)) : pos(pos),
                                                                                                                                              rot(rot),
                                                                                                                                              scale(scale) {}
    inline glm::mat4 Model() const
    {
        glm::mat4 posMatrix = glm::translate(pos);
        glm::mat4 rotMatrix = glm::toMat4(glm::quat(glm::vec3(rot.x, rot.y, rot.z)));
        glm::mat4 scaleMatrix = glm::scale(scale);

        return posMatrix * rotMatrix * scaleMatrix;
    }

    inline glm::vec3 &Pos() { return pos; }
    inline glm::vec3 &Rot() { return rot; }
    inline glm::vec3 &Scale() { return scale; }

    inline void SetPos(const glm::vec3 &arg) { pos = arg; }
    inline void SetRot(const glm::vec3 &arg) { rot = arg; }
    inline void SetScale(const glm::vec3 &arg) { scale = arg; }

protected:
private:
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
};

#endif // TRANSFORM_H