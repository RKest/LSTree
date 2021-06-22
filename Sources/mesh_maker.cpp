#include "mesh_maker.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>

#define TRANSLATION_AMOUNT 1.0

std::unique_ptr<ColouredMesh> MeshMaker::TreeMesh()
{
    return std::make_unique<ColouredMesh>(&positions[0], &colours[0], positions.size(), &indicies[0], indicies.size());
}

std::unique_ptr<ColouredMesh> MeshMaker::StemMesh()
{
    return std::make_unique<ColouredMesh>(&stemPositions[0], &stemColours[0], stemPositions.size(), &stemIndices[0], stemIndices.size());
}

std::unique_ptr<ColouredMesh> MeshMaker::BladeMesh()
{
    return std::make_unique<ColouredMesh>(&bladePositions[0], &bladeColours[0], bladePositions.size(), &bladeIndices[0], bladeIndices.size());
}

MeshMaker::MeshMaker(ui seed, const glm::mat4 &_baseTransform, ui _noWalls, const std::string &lsString) : baseTransform(_baseTransform), noWalls(_noWalls), rootJoint(NULL)
{
    customRand = CustomRand(seed);

    //Joint Shenanigans
    baseTransformInverse = glm::inverse(baseTransform);
    ui stringLength = lsString.length();

    PushNewState(0, BROWN, 0, baseTransform);
    VerticifyMesh(SEGMENT_GURTH_INTERCEPT, COLOURS[BROWN], baseTransform);
    LoadLeafModel();

    for (ui i = 0; i < stringLength; i++)
        switch (lsString[i])
        {
        case * "F":
        {
            ui segmentLength = GetSegmentLength(lsString, i);
            i += segmentLength - 1;
            HandleF(segmentLength);
            break;
        }
        case * "+":
            HandlePlus();
            break;
        case * "-":
            HandleMinus();
            break;
        case * "[":
            HandleLeftBracket();
            break;
        case * "]":
            HandleRightBracket();
            break;
        case * "C":
            HandleC();
            break;
        case * "G":
            HandleG();
            break;
        default:
            break;
        }
}

MeshMaker::~MeshMaker()
{
    std::cout << "Maker Destructed" << std::endl;
}

// glm::mat4 MeshMaker::RandomRotationMatrix()
// {
//     f32 r = static_cast<f32>(rand());
//     glm::mat4 xMat = glm::rotate(sinf(r), glm::vec3(1, 0, 0));
//     glm::mat4 yMat = glm::rotate(cosf(r), glm::vec3(0, 0, 1));
//     return xMat * yMat;
// }

void MeshMaker::HandleF(ui noSegments)
{
    LSystemState *topState = &states.top();
    ui *level = &topState->level;

    if (*level != newestLevel)
        level = &newestLevel;

    topState->gurthExponent += noSegments >> 1;
    topState->transform *= translationMatrix(noSegments);
    f radius = GetGurthByCoefficientComponent(topState->gurthExponent);

    VerticifyMesh(radius, colours[topState->colour], topState->transform);
    IndexMesh(topState->level, *level + 1, 0, noWalls);

    topState->level = *level;
    topState->level++;
    newestLevel++;

    if (Joint::maxNoJoints > newestJoint)
    {
        auto jointPtr = std::make_unique<Joint>();
        jointPtr->id = ++newestJoint;
        jointPtr->jointTransform = topState->transform;

        if (topState->parentJointPtr != NULL)
            topState->parentJointPtr->childJoints.push_back(*jointPtr);
        else

            topState->parentJointPtr = std::move(jointPtr);
    }
}

void MeshMaker::HandleMinus()
{
    glm::mat4 rotTransform = RotateByDegrees(-SEGMENT_ROTATION);
    states.top().transform *= rotTransform;
}

void MeshMaker::HandlePlus()
{
    glm::mat4 rotTransform = RotateByDegrees(SEGMENT_ROTATION);
    states.top().transform *= rotTransform;
}

void MeshMaker::HandleLeftBracket()
{
    states.top().gurthExponent *= 1.3;
    PushNewState();
}

void MeshMaker::HandleRightBracket()
{
    states.pop();
}

void MeshMaker::HandleC()
{
    states.top().transform *= translationMatrix(1);
    LoadLeaf(states.top().transform, leafIndex++);
}

void MeshMaker::HandleG()
{
    states.top().gurthExponent++;
}

/*
Indexes the verticies such that they describe a enclosed by <noWalls> space
Each wall is made with 2 triangles indexed counter-clockwise
*/
void MeshMaker::IndexMesh(ui fromLevel, ui toLevel, ui fromWall, ui toWall)
{
    ui level_walls, first_and_forth, second_and_sixth, last_el_overflow, level_interval, interval_walls;
    level_interval = toLevel - fromLevel;
    for (ui i = fromWall; i < toWall; ++i)
    {
        level_walls = fromLevel * noWalls;
        interval_walls = level_interval * noWalls + level_walls;
        last_el_overflow = (i + 1) * ((i + 1) != noWalls);

        first_and_forth = interval_walls + last_el_overflow;
        second_and_sixth = i + level_walls;

        indicies.push_back(first_and_forth);
        indicies.push_back(second_and_sixth);
        indicies.push_back(interval_walls + i);
        indicies.push_back(first_and_forth);
        indicies.push_back(level_walls + last_el_overflow);
        indicies.push_back(second_and_sixth);
    }
}

void MeshMaker::VerticifyMesh(f radius, const glm::vec3 &colour, const glm::mat4 &transform)
{
    f sinVal, cosVal;
    ui firstAngle = 360 / noWalls;

    if (cosSinMemTable[firstAngle].Repr())
    {
        for (ui i = 0; i <= 360 - firstAngle; i += firstAngle)
        {
            sinVal = cosSinMemTable[i].sinVal;
            cosVal = cosSinMemTable[i].cosVal;
            positions.push_back(transform * glm::vec4(cosVal * radius, 0, sinVal * radius, TRANSLATION_AMOUNT));
            colours.push_back(colour);
        }
    }
    else
    {
        for (ui i = 0; i <= 360 - firstAngle; i += firstAngle)
        {
            sinVal = sin(glm::radians((f)i));
            cosVal = cos(glm::radians((f)i));
            cosSinMemTable[i].sinVal = sinVal;
            cosSinMemTable[i].cosVal = cosVal;
            positions.push_back(transform * glm::vec4(cosVal * radius, 0, sinVal * radius, TRANSLATION_AMOUNT));
            colours.push_back(colour);
        }
    }
}

void MeshMaker::LoadLeafModel()
{
    leafStemModel = OBJModel("./Resources/obj/Leaf/stem.obj").ToIndexedModel();
    leafBladeModel = OBJModel("./Resources/obj/Leaf/blade.obj").ToIndexedModel();
}

void MeshMaker::LoadLeaf(const glm::mat4 &transform, ui leafIndex)
{
    for (ui i = 0; i < leafStemModel.positions.size(); ++i)
    {
        glm::vec3 *pos = &leafStemModel.positions[i];
        stemPositions.push_back(transform * glm::vec4(pos->x, pos->y, pos->z, TRANSLATION_AMOUNT));
        stemColours.push_back(COLOURS[BROWN]);
    }

    for (ui i = 0; i < leafStemModel.indices.size(); ++i)
    {
        stemIndices.push_back(leafStemModel.indices[i] + leafStemModel.indices.size() * leafIndex);
    }

    for (ui i = 0; i < leafBladeModel.positions.size(); ++i)
    {
        glm::vec3 *pos = &leafBladeModel.positions[i];
        bladePositions.push_back(transform * glm::vec4(pos->x, pos->y, pos->z, TRANSLATION_AMOUNT));
        bladeColours.push_back(COLOURS[GREEN]);
    }

    for (ui i = 0; i < leafBladeModel.indices.size(); ++i)
    {
        bladeIndices.push_back(leafBladeModel.indices[i] + leafBladeModel.indices.size() * leafIndex);
    }
}

void MeshMaker::PushNewState(ui level, ui colourIndex, ui gurthCoefficientComponent, glm::mat4 &transform)
{
    LSystemState state;
    state.level = level;
    state.transform = transform;
    state.colour = colourIndex;
    state.gurthExponent = gurthCoefficientComponent;
    state.parentJointPtr = NULL;

    states.push(state);
}

void MeshMaker::PushNewState()
{
    states.push(states.top());
}

glm::mat4 MeshMaker::RotateByDegrees(f deg)
{
    f r = customRand.NextFloat(0.0, 1.0);
    return glm::rotate(glm::radians(deg), glm::vec3(cosf(r), r, sinf(r)));
}

f MeshMaker::GetGurthByCoefficientComponent(ui gurthExponent)
{
    return SEGMENT_GURTH_CONSTANT + SEGMENT_GURTH_INTERCEPT * pow(SEGMENT_GURTH_DECAY_RATE, gurthExponent);
}

ui MeshMaker::GetSegmentLength(const std::string &stateString, ui i)
{
    ui segmentLength = 0;
    while (stateString[i] == *"F")
    {
        segmentLength++;
        i++;
    }
    return segmentLength;
}

glm::mat4 MeshMaker::translationMatrix(ui noSegments)
{
    return glm::translate(glm::vec3(0, SEGMNET_LENGTH * noSegments, 0));
}

// glm::mat4 MeshMaker::blankTransform = glm::mat4(
//     glm::vec4(1, 0, 0, 0),
//     glm::vec4(0, 1, 0, 0),
//     glm::vec4(0, 0, 1, 0),
//     glm::vec4(0, 0, 0, 1));

// glm::mat4 MeshMaker::blankTransform = glm::translate(glm::vec3(0, -1, 0));
