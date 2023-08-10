#ifndef MESH_H
#define MESH_H

#include "Ext/obj_loader.h"
#include <GL/glew.h>
#include <algorithm>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <span>
#include <string>

#include "_config.h"
#include "boost/pfr/core.hpp"
#include "joint.h"
#include "mesh_data.hpp"
#include "shader.h"

class Mesh
{
public:
    template<mesh_mods::MeshMods Mods> Mesh(MeshData<Mods> mesh_data);
    void Draw();
    template<typename Element>
    auto initVertexData(std::span<Element> elements, GLuint& buffer_pos) -> bool;
    void initNormals(std::span<glm::vec3> normals, std::span<glm::vec3> positions,
                     std::span<unsigned> indices, GLuint& buffer_pos);
    void initIndices(std::span<unsigned> indices, GLuint buffer_pos);

private:
    std::size_t drawCount;
    std::vector<GLuint> vertexArrayBuffers;
    GLuint vertexArrayObject;
};

template<mesh_mods::MeshMods Mods>
Mesh::Mesh(MeshData<Mods> mesh_data) :
    drawCount{mesh_data.indices.size()}, vertexArrayBuffers(drawCount)
{
    std::vector<glm::vec3> calced_normals;
    using boost::pfr::for_each_field;
    GLuint buffer_position = 0;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    glGenBuffers(mesh_data.getNumberOfBuffers(), vertexArrayBuffers.data());
    initVertexData(mesh_data.vertices, buffer_position);
    initIndices(mesh_data.indices, buffer_position++);
    initNormals(mesh_data.normals, mesh_data.vertices, mesh_data.indices, buffer_position);
    initVertexData(mesh_data.colors, buffer_position);
    initVertexData(mesh_data.joint_indices, buffer_position);
    glBindVertexArray(0);
}

template<typename Datum>
auto Mesh::initVertexData(std::span<Datum> data, GLuint& buffer_pos) -> bool
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[buffer_pos]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Datum), data.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(buffer_pos);
    glVertexAttribPointer(buffer_pos, sizeof(Datum) / sizeof(float), GL_FLOAT, GL_FALSE, 0, 0);
    buffer_pos++;
    return true;
}

#endif // MESH_H