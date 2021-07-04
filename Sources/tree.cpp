#include "tree.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>

#define TRANSLATION_AMOUNT 1.0

std::unique_ptr<AnimatedColouredMesh> Tree::BarkMesh()
{
    return std::make_unique<AnimatedColouredMesh>(
        &positions[0], &colours[0], positions.size(), 
        &indicies[0], indicies.size(), &jointIndices[0]);
}

std::unique_ptr<AnimatedColouredMesh> Tree::StemMesh()
{
    return std::make_unique<AnimatedColouredMesh>(
        &stemPositions[0], &stemColours[0], stemPositions.size(), 
        &stemIndices[0], stemIndices.size(), &stemJointIndices[0]);
}

std::unique_ptr<AnimatedColouredMesh> Tree::BladeMesh()
{
    return std::make_unique<AnimatedColouredMesh>(
        &bladePositions[0], &bladeColours[0], bladePositions.size(), 
        &bladeIndices[0], bladeIndices.size(), &bladeJointIndices[0]);
}

Tree::Tree(const glm::mat4 &_baseTransform, ui _noWalls, const std::string &lsString, CustomRand &_customRand, Joint &_rootJoint)
: baseTransform(_baseTransform), baseTransformInverse(glm::inverse(_baseTransform)), noWalls(_noWalls), customRand(_customRand), rootJoint(_rootJoint)
{
    ui stringLength = lsString.length();

    LSystemState initialState;

    initialState.level = 0;
    initialState.colour = BROWN;
    initialState.gurthExponent = 0;
    initialState.transform = _baseTransform;
    initialState.youngestJoint = &rootJoint;
    initialState.jointCapacity = MAX_JOINT_DEPTH;

    VerticifyMesh(initialState);
    LoadLeafModel();

    states.push(std::move(initialState));

    for (ui i = 0; i < stringLength; ++i)
        switch (lsString[i])
        {
        case * "F":
        {
            ui noSegments = SegmentLength(lsString, i);
            glm::mat4 localTransform = TranslationMatrix(noSegments);
            
            i += noSegments - 1;
            HandleF(noSegments, localTransform);
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

Tree::~Tree()
{
    std::cout << "Tree Destructed" << std::endl;
}

void Tree::HandleF(ui noSegments, const glm::mat4 &localTransform)
{
    LSystemState *topState = &states.top();
    ui *level = &topState->level;

    if (*level != newestLevel)
        level = &newestLevel;

    topState->gurthExponent += noSegments >> 1;
    topState->transform *= localTransform;
    ft radius = GurthByExponent(topState->gurthExponent);

    VerticifyMesh(*topState);
    IndexMesh(topState->level, *level + 1, 0, noWalls);

    topState->level = *level;
    topState->level++;
    newestLevel++;

    if (topState->jointCapacity)
    {
        --topState->jointCapacity;
        Joint *jointPtr = new Joint(topState->transform, ++newestJoint, topState->youngestJoint);
        topState->youngestJoint->childJointPtrs.push_back(jointPtr);
        topState->youngestJoint = jointPtr;
    }
}

void Tree::HandleMinus()
{
    states.top().transform *= RotateByDegrees(-SEGMENT_ROTATION);
}

void Tree::HandlePlus()
{
    states.top().transform *= RotateByDegrees(SEGMENT_ROTATION);
}

void Tree::HandleLeftBracket()
{
    states.top().gurthExponent *= 1.3;
    states.push(states.top());
}

void Tree::HandleRightBracket()
{
    states.pop();
}

void Tree::HandleC()
{
    states.top().transform *= TranslationMatrix(1);
    LoadLeaf(states.top().transform, leafIndex++, states.top().youngestJoint->AffectedIndices());
}

void Tree::HandleG()
{
    states.top().gurthExponent++;
}

/*
Indexes the verticies such that they describe a enclosed by <noWalls> space
Each wall is made with 2 triangles indexed counter-clockwise
*/
void Tree::IndexMesh(ui fromLevel, ui toLevel, ui fromWall, ui toWall)
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

void Tree::VerticifyMesh(const LSystemState &topState)
{
    ft sinVal, cosVal;
    ui firstAngle = 360 / noWalls;

    glm::ivec3 jointIndicesVec = topState.youngestJoint->AffectedIndices();
    // glm::ivec3 jointIndicesVec = glm::ivec3(0);
    ft radius = GurthByExponent(topState.gurthExponent);

    if (cosSinMemTable[firstAngle].Repr())
    {
        for (ui i = 0; i <= 360 - firstAngle; i += firstAngle)
        {
            sinVal = cosSinMemTable[i].sinVal;
            cosVal = cosSinMemTable[i].cosVal;
            positions.push_back(topState.transform * glm::vec4(cosVal * radius, 0, sinVal * radius, TRANSLATION_AMOUNT));
            colours.push_back(COLOURS[topState.colour]);
            jointIndices.push_back(jointIndicesVec);
        }
    }
    else
    {
        for (ui i = 0; i <= 360 - firstAngle; i += firstAngle)
        {
            sinVal = sin(glm::radians((ft)i));
            cosVal = cos(glm::radians((ft)i));
            cosSinMemTable[i].sinVal = sinVal;
            cosSinMemTable[i].cosVal = cosVal;
            positions.push_back(topState.transform * glm::vec4(cosVal * radius, 0, sinVal * radius, TRANSLATION_AMOUNT));
            colours.push_back(COLOURS[topState.colour]);
            jointIndices.push_back(jointIndicesVec);
        }
    }
}

void Tree::LoadLeafModel()
{
    leafStemModel = OBJModel("./Resources/obj/Leaf/stem.obj").ToIndexedModel();
    leafBladeModel = OBJModel("./Resources/obj/Leaf/blade.obj").ToIndexedModel();
}

void Tree::LoadLeaf(const glm::mat4 &transform, ui leafIndex, const glm::ivec3 &_jointIndices)
{
    for (ui i = 0; i < leafStemModel.positions.size(); ++i)
    {
        glm::vec3 *pos = &leafStemModel.positions[i];
        stemPositions.push_back(transform * glm::vec4(pos->x, pos->y, pos->z, TRANSLATION_AMOUNT));
        stemColours.push_back(COLOURS[BROWN]);
        stemJointIndices.push_back(_jointIndices);
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
        bladeJointIndices.push_back(_jointIndices);
    }

    for (ui i = 0; i < leafBladeModel.indices.size(); ++i)
    {
        bladeIndices.push_back(leafBladeModel.indices[i] + leafBladeModel.indices.size() * leafIndex);
    }
}

glm::mat4 Tree::RotateByDegrees(ft deg)
{
    ft r = customRand.NextFloat(0.0, 1.0);
    return glm::rotate(glm::radians(deg), glm::vec3(cosf(r), r, sinf(r)));
}

ft Tree::GurthByExponent(ui gurthExponent)
{
    return SEGMENT_GURTH_CONSTANT + SEGMENT_GURTH_INTERCEPT * pow(SEGMENT_GURTH_DECAY_RATE, gurthExponent);
}

ui Tree::SegmentLength(const std::string &stateString, ui i)
{
    ui segmentLength = 0;
    while (stateString[i] == *"F")
    {
        segmentLength++;
        i++;
    }
    return segmentLength;
}

glm::mat4 Tree::TranslationMatrix(ui noSegments)
{
    return glm::translate(glm::vec3(0, SEGMNET_LENGTH * noSegments, 0));
}
