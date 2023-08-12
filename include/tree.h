#ifndef MESH_MAKER_H
#define MESH_MAKER_H

#include "Ext/obj_loader.h"
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include <memory>
#include <stack>

#include "_config.h"
#include "mesh.h"
#include "random.h"

struct LSystemState
{
    glm::mat4 transform;
    ui level;
    ui colour;
    ui gurthExponent;
    std::shared_ptr<Joint> youngestJoint;
    ui jointCapacity;
};

class Tree
{
public:
    Tree(const glm::mat4& _baseTransform, ui _noWalls, const std::string& lsString,
         CustomRand& _customRand, Joint* _rootJoint);

    std::unique_ptr<Mesh> BarkMesh();
    std::unique_ptr<Mesh> StemMesh();
    std::unique_ptr<Mesh> BladeMesh();

    std::unique_ptr<std::vector<std::shared_ptr<Joint>>> JointPtrVectorPtr();

    ~Tree();

protected:
private:
    void VerticifyMesh(const LSystemState& topState);
    void IndexMesh(ui fromLevel, ui toLevel, ui fromWall, ui toWall);

    CustomRand& customRand;
    std::shared_ptr<Joint> rootJointPtr;

    IndexedModel leafStemModel;
    IndexedModel leafBladeModel;

    glm::mat4 baseTransform;
    glm::mat4 baseTransformInverse;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> stemPositions;
    std::vector<glm::vec3> bladePositions;

    std::vector<glm::vec3> colours;
    std::vector<glm::vec3> stemColours;
    std::vector<glm::vec3> bladeColours;

    std::vector<ui> indicies;
    std::vector<ui> stemIndices;
    std::vector<ui> bladeIndices;

    std::vector<glm::ivec3> jointIndices;
    std::vector<glm::ivec3> stemJointIndices;
    std::vector<glm::ivec3> bladeJointIndices;

    ui noLevels;
    ui noWalls;

    std::stack<LSystemState> states;
    ui newestLevel = 0;
    ui newestJoint = 0;

    void HandleF(ui noSegments, const glm::mat4& localTransform);
    void HandlePlus();
    void HandleMinus();
    void HandleLeftBracket();
    void HandleRightBracket();
    void HandleC();
    void HandleG();

    void LoadLeafModel();
    void LoadLeaf(const glm::mat4& transform, ui leafIndex, const glm::ivec3& _jointIndices);

    ft GurthByExponent(ui gurthExponent);
    ui SegmentLength(const std::string& stateString, ui i);
    glm::mat4 RotateByDegrees(ft deg);
    glm::mat4 TranslationMatrix(ui noSegments);

    enum Coluors
    {
        GREEN,
        BROWN,

        NO_COLOURS
    };

    glm::vec3 COLOURS[NO_COLOURS] = {glm::vec3(0.13, 0.54, 0.13), glm::vec3(0.31, 0.14, 0.07)};

    // Leafs
    ui leafIndex = 0;

    // 361 as of 360 possible angles to memoize indexed by the angle value
};

#endif