#include "mesh.h"
#include <vector>

namespace
{
    auto calcNormals(std::span<glm::vec3> vertices, std::span<unsigned int> indices)
        -> std::vector<glm::vec3>
    {
        std::vector<glm::vec3> normals{vertices.size()};
        for (std::size_t i = 0; i < indices.size(); i += 3)
        {
            std::size_t i0 = indices[i];
            std::size_t i1 = indices[i + 1];
            std::size_t i2 = indices[i + 2];

            glm::vec3 v1 = vertices[i1] - vertices[i0];
            glm::vec3 v2 = vertices[i2] - vertices[i0];

            glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

            normals[i0] += normal;
            normals[i1] += normal;
            normals[i2] += normal;
        }

        for (std::size_t i = 0; i < vertices.size(); i++)
            normals[i] = glm::normalize(normals[i]);
        return normals;
    }
} // namespace

void Mesh::Draw()
{
    glBindVertexArray(vertexArrayObject);
    glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::initVertexArrayWith(std::size_t number_of_buffers)
{
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    glGenBuffers(number_of_buffers, vertexArrayBuffers.data());
}

void Mesh::initIndices(std::span<unsigned> indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[next_buffer_pos++]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), indices.data(),
                 GL_STATIC_DRAW);
}

void Mesh::initNormals(std::span<glm::vec3> normals, std::span<glm::vec3> positions,
                       std::span<unsigned int> indices)
{
    if (normals.empty())
    {
        auto calculated_normals = calcNormals(positions, indices);
        initVertexData(std::span{calculated_normals});
        return;
    }
    initVertexData(normals);
}