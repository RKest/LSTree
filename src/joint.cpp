#include "joint.h"

Joint::Joint(const glm::mat4& _baseTransform, unsigned _id,
             std::shared_ptr<Joint> _parentJointPtr) :
    id(_id),
    parentJointPtr(_parentJointPtr),
    baseTransform(_baseTransform),
    baseTransformInverse(glm::inverse(_baseTransform))
{
    if (_parentJointPtr != nullptr)
    {
        glm::mat4 toParentTransform = parentJointPtr->baseTransform * baseTransformInverse;
        toParentVector =
            glm::vec3(toParentTransform[3][0], toParentTransform[3][1], toParentTransform[3][2]);
    }
};

glm::ivec3 Joint::AffectedIndices()
{
    glm::ivec3 returnVector(-1);

    Joint* currentlyExaminedJoint = this;
    unsigned currentVectorIndex = 0;

    while (currentlyExaminedJoint->parentJointPtr != nullptr)
    {
        returnVector[currentVectorIndex++] = currentlyExaminedJoint->id;
        currentlyExaminedJoint = currentlyExaminedJoint->parentJointPtr.get();
    }

    return returnVector;
}

std::vector<std::shared_ptr<Joint>> Joint::ToJointPtrVector(std::shared_ptr<Joint> rootJoint)
{
    std::vector<std::shared_ptr<Joint>> returnVector;
    returnVector.push_back(rootJoint);
    RecurseChildren(rootJoint.get(), returnVector);
    returnVector.shrink_to_fit();
    std::sort(returnVector.begin(), returnVector.end(), CompareJoints);

    return returnVector;
}

void Joint::RecurseChildren(Joint* joint, std::vector<std::shared_ptr<Joint>>& jointVector)
{
    for (std::size_t i = 0; i < joint->childJointPtrs.size(); ++i)
    {
        jointVector.push_back(joint->childJointPtrs[i]);
        RecurseChildren(joint->childJointPtrs[i].get(), jointVector);
    }
}

bool Joint::CompareJoints(const std::shared_ptr<Joint>& joint1,
                          const std::shared_ptr<Joint>& joint2)
{
    return joint1->id < joint2->id;
}