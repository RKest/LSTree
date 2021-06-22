#ifndef MESH_MAKER_H
#define MESH_MAKER_H

#include "glm/glm.hpp"
#include "Ext/obj_loader.h"
#include <glm/gtx/transform.hpp>
#include <stack>
#include <memory>

#include "_config.h"
#include "mesh.h"
#include "random.h"

struct CosSinPair
{
    f cosVal;
    f sinVal;
    f Repr()
    {
        return cosVal + sinVal;
    }
};

struct LSystemState
{
    glm::mat4 transform;
    ui level;
    ui colour;
    ui gurthExponent;
    std::unique_ptr<Joint> parentJointPtr; 
};

class MeshMaker
{
public:
    MeshMaker(ui seed, const glm::mat4 &baseTransform, ui _noWalls, const std::string &lsString);

    std::unique_ptr<ColouredMesh> TreeMesh();
    std::unique_ptr<ColouredMesh> StemMesh();
    std::unique_ptr<ColouredMesh> BladeMesh();

    ~MeshMaker();

protected:
private:
    void VerticifyMesh(f radius, const glm::vec3 &colour, const glm::mat4 &transform);
    void IndexMesh(ui fromLevel, ui toLevel, ui fromWall, ui toWall);

    CustomRand customRand;
    Joint *rootJoint;

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

    ui noLevels;
    ui noWalls;
    f radius;

    //361 as of 360 possible angles to memoize indexed by the angle value
    CosSinPair cosSinMemTable[361] = {{0, 0}};

    glm::mat4 RandomRotationMatrix();
    f GetGurthByCoefficientComponent(ui gurthExponent);
    ui GetSegmentLength(const std::string &stateString, ui i);

    std::stack<LSystemState> states;
    ui newestLevel = 0;
    ui newestJoint = 0;

    void HandleF(ui noSegments);
    void HandlePlus();
    void HandleMinus();
    void HandleLeftBracket();
    void HandleRightBracket();
    void HandleC();
    void HandleG();

    enum Coluors
    {
        GREEN,
        BROWN,

        NO_COLOURS
    };

    void PushNewState();
    void PushNewState(ui level, ui colourIndex, ui gurthCoefficientComponent, glm::mat4 &transform);
    glm::mat4 RotateByDegrees(f deg);
    glm::mat4 translationMatrix(ui noSegments);

    glm::vec3 COLOURS[NO_COLOURS] = {
        glm::vec3(0.13, 0.54, 0.13),
        glm::vec3(0.31, 0.14, 0.07)};

    //Leafs
    ui leafIndex = 0;

    IndexedModel leafStemModel;
    IndexedModel leafBladeModel;

    void LoadLeafModel();
    void LoadLeaf(const glm::mat4 &transform, ui leafIndex);
};

#endif