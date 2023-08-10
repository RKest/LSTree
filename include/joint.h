#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

class Joint
{
public:
    Joint(const glm::mat4& baseTransform, unsigned _id, Joint* _parentJointPtr);
    glm::ivec3 AffectedIndices();

    glm::vec3 toParentVector;

    glm::mat4 baseTransform;
    glm::mat4 baseTransformInverse;

    std::vector<Joint*> childJointPtrs;
    static std::vector<Joint*> ToJointPtrVector(Joint* rootJoint);

    Joint* parentJointPtr;

    ~Joint();

private:
    unsigned id;

    glm::vec3 VectorBetweenTranslationMatrices(const glm::mat4& parentTransform,
                                               const glm::mat4& childTransform);
    static void RecurseChildren(Joint* joint, std::vector<Joint*>& jointVector);
    static bool CompareJoints(const Joint* joint1, const Joint* joint2);
};
