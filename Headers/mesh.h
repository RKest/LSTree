#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include "Ext/obj_loader.h"

#include "_config.h"
#include "shader.h"

struct PerformentIndexedModel
{
public:
    PerformentIndexedModel(
        glm::vec3 *_positions, glm::vec2 *_texCoords, glm::vec3 *_colours, ui *_indices,
        ui _noVertices, ui _noIndices,
        const std::vector<glm::vec3> *_normalsPtr = NULL,
        glm::ivec3 *_jointIndices = NULL
    )
    : positions(_positions), texCoords(_texCoords), colours(_colours), indices(_indices), noVertices(_noVertices), noIndices(_noIndices), jointIndices(_jointIndices)
    {
        if (_normalsPtr != NULL)
        {
            normalsPtr = _normalsPtr;
        }
        else
        {
            normals.resize(_noVertices, glm::vec3(0));
            CalcNormals();
            normalsPtr = &normals;
        }
    }

    const glm::vec3 *positions;
    const glm::vec2 *texCoords;
    const glm::vec3 *colours;
    const ui *indices;

    const ui noVertices;
    const ui noIndices;

    const std::vector<glm::vec3> *normalsPtr;
    const glm::ivec3 *jointIndices;

private:
    std::vector<glm::vec3> normals;
    void CalcNormals();
};

class Mesh
{
public:
    Mesh();
    void Draw();

protected:
    GLuint vertexArrayObject;
    void SetDrawCount(ui arg) { drawCount = arg; };
    void InitPositions(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitPositions(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitNormals(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitNormals(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitColours(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitColours(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitIndices(const PerformentIndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);
    void InitIndices(const IndexedModel &model, GLuint *vertexArrayBuffers, ui bufferPosition);

private:
    ui drawCount;
};

class UntexturedMesh : public Mesh
{
public:
    UntexturedMesh(glm::vec3 *positions, ui noVerticies, ui *indicies, ui noIndicies);
    UntexturedMesh(const std::string &fileName, bool calculateNormals);

protected:
private:
    void InitMesh(const PerformentIndexedModel &model);
    void InitMesh(const IndexedModel &model);
    enum
    {
        POSITION_VB,
        NORMAL_VB,
        INDEX_VB,

        NUM_BUFFERS
    };
    GLuint vertexArrayBuffers[NUM_BUFFERS];
};

class TexturedMesh : public Mesh
{
public:
    TexturedMesh(glm::vec3 *positions, glm::vec2 *texCoords, ui noVerticies, ui *indicies, ui noIndicies);
    TexturedMesh(const std::string &fileName, bool calculateNormals);

protected:
private:
    void InitMesh(const PerformentIndexedModel &model);
    void InitMesh(const IndexedModel &model);
    enum
    {
        POSITION_VB,
        NORMAL_VB,
        TEXCOORD_VB,
        INDEX_VB,

        NUM_BUFFERS
    };
    GLuint vertexArrayBuffers[NUM_BUFFERS];
};

class ColouredMesh : public Mesh
{
public:
    ColouredMesh(const std::string &fileName, const glm::vec3 &colour);
    ColouredMesh(glm::vec3 *positions, glm::vec3 *colours, ui noVertices, ui *indices, ui noIndices);
    ColouredMesh(glm::vec3 *positions, glm::vec3 *colours, ui noVertices, ui *indices, ui noIndices, const std::vector<glm::vec3> &normals);

private:
    void InitMesh(const PerformentIndexedModel &model);
    void InitMesh(const IndexedModel &model);
    enum
    {
        POSITION_VB,
        NORMAL_VB,
        COLOUR_VB,
        INDEX_VB,

        NUM_BUFFERS
    };

    GLuint vertexArrayBuffers[NUM_BUFFERS];
};

class Joint 
{
public:
    Joint(const glm::mat4 &baseTransform, ui _id, Joint *_parentJointPtr);
    glm::ivec3 AffectedIndices();

    glm::vec3 toParentVector;

    glm::mat4 baseTransform;
    glm::mat4 baseTransformInverse;

    std::vector<Joint *> childJointPtrs;
    static std::vector<Joint *> *ToJointPtrVector(Joint *rootJoint);

    Joint* parentJointPtr;

    ~Joint();
private:
    ui id;

    glm::vec3 VectorBetweenTranslationMatrices(const glm::mat4 &parentTransform, const glm::mat4 &childTransform);
    static void RecurseChildren(Joint *joint, std::vector<Joint *> *jointVector);
    static bool CompareJoints(const Joint *joint1, const Joint *joint2);
};

class AnimatedColouredMesh : public Mesh
{
public:
    AnimatedColouredMesh(glm::vec3 *positions, glm::vec3 *colours, ui noVertices, ui *indices, ui noIndices, glm::ivec3 *jointIndices);
private:
    void InitMesh(const PerformentIndexedModel &model);
    enum 
    {
        POSITION_VB,
        JOINT_INDEX_VB,
        NORMAL_VB,
        COLOUR_VB,
        INDEX_VB,

        NUM_BUFFERS
    };

    GLuint vertexArrayBuffers[NUM_BUFFERS];
};

#endif //MESH_H