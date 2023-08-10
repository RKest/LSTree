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
#include "joint.h"
#include "mesh_data.hpp"
#include "shader.h"

class Mesh
{
public:
    template<mesh_mods::MeshMods Mods> Mesh(MeshArgs<Mods> mesh_args);
    void Draw();
    template<typename Element> void initVertexData(std::span<Element> elements);
    void initVertexArrayWith(std::size_t number_of_buffers);
    void initNormals(std::span<glm::vec3> normals, std::span<glm::vec3> positions,
                     std::span<unsigned> indices);
    void initIndices(std::span<unsigned> indices);
    void initIfModded(auto&& may_data);

private:
    std::size_t drawCount;
    std::vector<GLuint> vertexArrayBuffers;
    GLuint vertexArrayObject;
    GLuint next_buffer_pos = 0;
};

template<mesh_mods::MeshMods Mods>
Mesh::Mesh(MeshArgs<Mods> mesh_args) :
    drawCount{mesh_args.getIndices().size()}, vertexArrayBuffers(drawCount)
{
    initVertexArrayWith(mesh_args.numberOfBuffers());
    initVertexData(mesh_args.getPositions());
    initIndices(mesh_args.getIndices());
    initNormals(mesh_args.getNormals(), mesh_args.getPositions(), mesh_args.getIndices());
    initIfModded(mesh_args.getColors());
    initIfModded(mesh_args.getTextures());
    initIfModded(mesh_args.getJointIndices());
    glBindVertexArray(0);
}

template<typename Datum> void Mesh::initVertexData(std::span<Datum> data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[next_buffer_pos]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Datum), data.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(next_buffer_pos);
    glVertexAttribPointer(next_buffer_pos, sizeof(Datum) / sizeof(float), GL_FLOAT, GL_FALSE, 0, 0);
    ++next_buffer_pos;
}

void Mesh::initIfModded(auto&& may_data)
{
    if constexpr (requires { initVertexData(std::forward<decltype(may_data)>(may_data)); })
    {
        initVertexData(std::forward<decltype(may_data)>(may_data));
    }
}

#endif // MESH_H