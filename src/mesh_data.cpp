#include "mesh_data.hpp"

#include "glm/geometric.hpp"

auto ls::calcNormals(std::span<glm::vec3> vertices, std::span<unsigned int> indices)
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
