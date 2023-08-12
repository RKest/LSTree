#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

class Joint
{
public:
    Joint(const glm::mat4& baseTransform, unsigned _id, std::shared_ptr<Joint> _parentJointPtr);
    glm::ivec3 AffectedIndices();

    glm::vec3 toParentVector;

    glm::mat4 baseTransform;
    glm::mat4 baseTransformInverse;

    std::vector<std::shared_ptr<Joint>> childJointPtrs;
    static std::vector<std::shared_ptr<Joint>> ToJointPtrVector(std::shared_ptr<Joint> rootJoint);

    std::shared_ptr<Joint> parentJointPtr;

private:
    unsigned id;

    glm::vec3 VectorBetweenTranslationMatrices(const glm::mat4& parentTransform,
                                               const glm::mat4& childTransform);
    static void RecurseChildren(Joint* joint, std::vector<std::shared_ptr<Joint>>& jointVector);
    static bool CompareJoints(const std::shared_ptr<Joint>& joint1,
                              const std::shared_ptr<Joint>& joint2);
};
