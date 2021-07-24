#include "mesh.h"
#include <vector>
#include "glm/gtx/string_cast.hpp"
#include <iostream>

Mesh::Mesh()
{
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
}

void Mesh::Draw()
{
    glBindVertexArray(vertexArrayObject);
    glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::InitPositions(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.noVertices, model.positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(bufferPosition);
    glVertexAttribPointer(bufferPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::InitPositions(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(bufferPosition);
    glVertexAttribPointer(bufferPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::InitNormals(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ARRAY_BUFFER, model.normalsPtr->size() * sizeof(glm::vec3), &(*model.normalsPtr)[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(bufferPosition);
    glVertexAttribPointer(bufferPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::InitNormals(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(bufferPosition);
    glVertexAttribPointer(bufferPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::InitColours(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ARRAY_BUFFER, model.noVertices * sizeof(model.colours[0]), model.colours, GL_STATIC_DRAW);

    glEnableVertexAttribArray(bufferPosition);
    glVertexAttribPointer(bufferPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::InitColours(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ARRAY_BUFFER, model.colours.size() * sizeof(model.colours[0]), &model.colours[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(bufferPosition);
    glVertexAttribPointer(bufferPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::InitIndices(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.noIndices, &model.indices[0], GL_STATIC_DRAW);
}

void Mesh::InitIndices(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[bufferPosition]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);
}

TexturedMesh::TexturedMesh(glm::vec3 *positions, glm::vec2 *texCoords, ui noVertices, ui *indices, ui noIndices)
{
    PerformentIndexedModel model(positions, texCoords, NULL, indices, noVertices, noIndices);
    InitMesh(model);
}

TexturedMesh::TexturedMesh(const std::string &fileName, bool calculateNormals)
{
    IndexedModel model = OBJModel(fileName).ToIndexedModel();
    if (calculateNormals)
    {
        model.CalcNormals();
    }

    InitMesh(model);
}

void TexturedMesh::InitMesh(const PerformentIndexedModel &model)
{
    SetDrawCount(model.noIndices);

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);
    InitNormals(model, vertexArrayBuffers, NORMAL_VB);

    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, model.noVertices * sizeof(model.texCoords[0]), model.texCoords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

void TexturedMesh::InitMesh(const IndexedModel &model)
{
    SetDrawCount(model.indices.size());

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);
    InitNormals(model, vertexArrayBuffers, NORMAL_VB);

    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

UntexturedMesh::UntexturedMesh(glm::vec3 *positions, ui noVerticies, ui *indices, ui noIndices)
{
    PerformentIndexedModel model(positions, NULL, NULL, indices, noVerticies, noIndices);
    InitMesh(model);
}

UntexturedMesh::UntexturedMesh(const std::string &fileName, bool calculateNormals)
{
    IndexedModel model = OBJModel(fileName).ToIndexedModel();

    if (calculateNormals)
        model.CalcNormals();
    InitMesh(model);
}

void UntexturedMesh::InitMesh(const PerformentIndexedModel &model)
{
    SetDrawCount(model.noIndices);

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);
    InitNormals(model, vertexArrayBuffers, NORMAL_VB);
    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

void UntexturedMesh::InitMesh(const IndexedModel &model)
{
    SetDrawCount(model.indices.size());

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);
    InitNormals(model, vertexArrayBuffers, NORMAL_VB);
    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

ColouredMesh::ColouredMesh(const std::string &fileName, const glm::vec3 &colour)
{
    IndexedModel model = OBJModel(fileName).ToIndexedModel();

    for (ui i = 0; i < model.positions.size(); i++)
    {
        model.normals[i] = glm::vec3(0, 0, 0);
        model.colours.push_back(colour);
    }

    model.CalcNormals();
    InitMesh(model);
}

ColouredMesh::ColouredMesh(glm::vec3 *positions, glm::vec3 *colours, ui noVertices, ui *indices, ui noIndices)
{
    PerformentIndexedModel model(positions, NULL, colours, indices, noVertices, noIndices);
    InitMesh(model);
}

ColouredMesh::ColouredMesh(glm::vec3 *positions, glm::vec3 *colours, ui noVertices, ui *indices, ui noIndices, const std::vector<glm::vec3> &normals)
{
    PerformentIndexedModel model(positions, NULL, colours, indices, noVertices, noIndices, &normals);
    InitMesh(model);
}

void ColouredMesh::InitMesh(const PerformentIndexedModel &model)
{
    SetDrawCount(model.noIndices);

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);
    InitNormals(model, vertexArrayBuffers, NORMAL_VB);
    InitColours(model, vertexArrayBuffers, COLOUR_VB);
    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

void ColouredMesh::InitMesh(const IndexedModel &model)
{
    SetDrawCount(model.indices.size());

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);
    InitNormals(model, vertexArrayBuffers, NORMAL_VB);
    InitColours(model, vertexArrayBuffers, COLOUR_VB);
    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

void PerformentIndexedModel::CalcNormals()
{
    for (ui i = 0; i < noIndices; i += 3)
    {
        ui i0 = indices[i];
        ui i1 = indices[i + 1];
        ui i2 = indices[i + 2];

        glm::vec3 v1 = positions[i1] - positions[i0];
        glm::vec3 v2 = positions[i2] - positions[i0];

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }

    for (ui i = 0; i < noVertices; i++)
        normals[i] = glm::normalize(normals[i]);
}

Joint::Joint(const glm::mat4 &_baseTransform, ui _id, Joint *_parentJointPtr)
    : id(_id), parentJointPtr(_parentJointPtr), baseTransform(_baseTransform), baseTransformInverse(glm::inverse(_baseTransform))
{
    if (_parentJointPtr != nullptr)
    {
        glm::mat4 toParentTransform = parentJointPtr->baseTransform * baseTransformInverse;
        toParentVector = glm::vec3(
            toParentTransform[3][0],
            toParentTransform[3][1],
            toParentTransform[3][2]);
    }
};

glm::ivec3 Joint::AffectedIndices()
{
    glm::ivec3 returnVector(-1);

    Joint *currentlyExaminedJoint = this;
    ui currentVectorIndex = 0;

    while (currentlyExaminedJoint->parentJointPtr != nullptr)
    {
        returnVector[currentVectorIndex++] = currentlyExaminedJoint->id;
        currentlyExaminedJoint = currentlyExaminedJoint->parentJointPtr;
    }

    return returnVector;
}

std::vector<Joint *> *Joint::ToJointPtrVector(Joint *rootJoint)
{
    std::vector<Joint *> *returnVector = new std::vector<Joint *>;
    returnVector->push_back(rootJoint);
    RecurseChildren(rootJoint, returnVector);
    returnVector->shrink_to_fit();
    std::sort(returnVector->begin(), returnVector->end(), CompareJoints);

    return returnVector;
}

void Joint::RecurseChildren(Joint *joint, std::vector<Joint *> *jointVector)
{
    for (ui i = 0; i < joint->childJointPtrs.size(); ++i)
    {
        jointVector->push_back(joint->childJointPtrs[i]);
        RecurseChildren(joint->childJointPtrs[i], jointVector);
    }
}

bool Joint::CompareJoints(const Joint *joint1, const Joint *joint2)
{
    return joint1->id < joint2->id;
}

AnimatedColouredMesh::AnimatedColouredMesh(glm::vec3 *positions, glm::vec3 *colours, ui noVertices, ui *indices, ui noIndices, glm::ivec3 *jointIndices)
{
    PerformentIndexedModel model(positions, NULL, colours, indices, noVertices, noIndices, NULL, jointIndices);
    InitMesh(model);
}

void AnimatedColouredMesh::InitMesh(const PerformentIndexedModel &model)
{
    SetDrawCount(model.noIndices);

    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

    InitPositions(model, vertexArrayBuffers, POSITION_VB);

    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[JOINT_INDEX_VB]);
    glBufferData(GL_ARRAY_BUFFER, model.noVertices * sizeof(model.jointIndices[0]), model.jointIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(JOINT_INDEX_VB);
    //This _______|_______ character right here was a difference between me having a weekend and not. Read documentation kids :)
    glVertexAttribIPointer(JOINT_INDEX_VB, 3, GL_UNSIGNED_INT, 0, 0);

    InitNormals(model, vertexArrayBuffers, NORMAL_VB);
    InitColours(model, vertexArrayBuffers, COLOUR_VB);

    InitIndices(model, vertexArrayBuffers, INDEX_VB);

    glBindVertexArray(0);
}

Joint::~Joint()
{
    for (auto j : childJointPtrs)
        delete j;
}