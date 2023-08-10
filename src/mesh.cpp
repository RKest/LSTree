#include "mesh.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <vector>

void Mesh::Draw()
{
    glBindVertexArray(vertexArrayObject);
    glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::initIndices(std::span<unsigned> indices, GLuint bufferPosition)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), indices.data(),
                 GL_STATIC_DRAW);
}

void Mesh::initNormals(std::span<glm::vec3> normals, std::span<glm::vec3> positions,
                       std::span<unsigned int> indices, GLuint& buffer_pos)
{
    if (normals.empty())
    {
        auto calced_normals = calcNormals(positions, indices);
        initVertexData(std::span{calced_normals}, buffer_pos);
        return;
    }
    initVertexData(normals, buffer_pos);
}

// void AnimatedColouredMesh::InitMesh(const PerformentIndexedModel &model)
//{
//     SetDrawCount(model.noIndices);
//
//     glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);
//
//     InitPositions(model, vertexArrayBuffers, POSITION_VB);
//
//     glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[JOINT_INDEX_VB]);
//     glBufferData(GL_ARRAY_BUFFER, model.noVertices * sizeof(model.jointIndices[0]),
//     model.jointIndices, GL_STATIC_DRAW);
//
//     glEnableVertexAttribArray(JOINT_INDEX_VB);
//     //This _______|_______ character right here was a difference between me having a weekend and
//     not. Read documentation kids :) glVertexAttribIPointer(JOINT_INDEX_VB, 3, GL_UNSIGNED_INT, 0,
//     0);
//
//     InitNormals(model, vertexArrayBuffers, NORMAL_VB);
//     InitColours(model, vertexArrayBuffers, COLOUR_VB);
//
//     InitIndices(model, vertexArrayBuffers, INDEX_VB);
//
//     glBindVertexArray(0);
// }

Joint::~Joint()
{
    for (auto j : childJointPtrs)
        delete j;
}