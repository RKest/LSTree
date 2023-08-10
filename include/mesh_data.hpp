#pragma once

#include <bitset>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <span>
#include <vector>

#define LS_TREE_MESH_EXT(TYPE, EXT_NAME, FIELD_NAME)                                               \
    struct EXT_NAME                                                                                \
    {                                                                                              \
        std::span<TYPE> FIELD_NAME;                                                                \
    }

LS_TREE_MESH_EXT(glm::vec3, Colors, colors);

namespace mesh_mods
{
    enum MeshMods
    {
        None = 0,
        Color = 1 << 1,
        Texture = 1 << 2,
        JointIndices = 1 << 3,
        Normals = 1 << 4,
    };
} // namespace mesh_mods

struct None
{
};

namespace ls
{
    auto calcNormals(std::span<glm::vec3> vertices, std::span<unsigned> indices)
        -> std::vector<glm::vec3>;
}
using namespace ls;

template<mesh_mods::MeshMods Mods = mesh_mods::None> class MeshData
{
    template<mesh_mods::MeshMods ModsQuery, typename Type>
    using SupportsOrNone = std::conditional_t<(ModsQuery | Mods) == 0, None, Type>;

public:
    std::span<glm::vec3> vertices;
    std::span<unsigned> indices;
    [[no_unique_address]] SupportsOrNone<mesh_mods::Color, std::span<glm::vec3>> colors;
    [[no_unique_address]] SupportsOrNone<mesh_mods::Texture, std::span<glm::vec2>> textures;
    [[no_unique_address]] SupportsOrNone<mesh_mods::JointIndices, std::span<glm::ivec3>>
        joint_indices;
    [[no_unique_address]] SupportsOrNone<mesh_mods::Normals, std::span<glm::vec3>> normals;

    auto getNumberOfBuffers() const -> std::size_t
    {
        std::bitset<64> set{static_cast<uint64_t>(Mods)};
        return set.count() + 3;
    }
};

template<mesh_mods::MeshMods Mods = mesh_mods::None> class MeshArgs
{
    consteval static auto with(mesh_mods::MeshMods new_mods) -> mesh_mods::MeshMods
    {
        return static_cast<mesh_mods::MeshMods>(Mods | new_mods);
    }

    MeshData<Mods> data;

public:
    MeshArgs(const MeshArgs&) = default;
    template<mesh_mods::MeshMods NewMods>
    MeshArgs(const MeshData<NewMods>& new_data) : data{new_data} {};
    auto operator=(const MeshArgs&) -> MeshArgs& = default;
    MeshArgs(std::span<glm::vec3> new_positions, std::span<unsigned> new_indices)
    {
        data.vertices = new_positions;
        data.indices = new_indices;
    };

    auto getData() const -> MeshData<Mods> const& { return data; }

    auto withColors(std::span<glm::vec3> colors) const -> MeshArgs<with(mesh_mods::Color)>
        requires((Mods & mesh_mods::Color) == 0)
    {
        MeshData<with(mesh_mods::Color)> new_data;
        new_data.colors = colors;
        new_data.normals = data.normals;
        new_data.vertices = data.vertices;
        new_data.indices = data.indices;
        new_data.joint_indices = data.joint_indices;
        new_data.textures = data.textures;
        return MeshArgs<with(mesh_mods::Color)>{new_data};
    }

    auto withTextures(std::span<glm::vec2> textures) -> MeshArgs<with(mesh_mods::Texture)>
        requires((Mods & mesh_mods::Texture) == 0)
    {
        MeshData<with(mesh_mods::Texture)> new_data;
        new_data.textures = textures;
        new_data.normals = data.normals;
        new_data.colors = data.colors;
        new_data.vertices = data.vertices;
        new_data.indices = data.indices;
        new_data.joint_indices = data.joint_indices;
        return MeshArgs<with(mesh_mods::Texture)>{new_data};
    }

    auto withJointIndices(std::span<glm::ivec3> joint_indices)
        -> MeshArgs<with(mesh_mods::JointIndices)>
        requires((Mods & mesh_mods::JointIndices) == 0)
    {
        MeshData<with(mesh_mods::JointIndices)> new_data;
        new_data.joint_indices = joint_indices;
        new_data.normals = data.normals;
        new_data.colors = data.colors;
        new_data.vertices = data.vertices;
        new_data.indices = data.indices;
        new_data.textures = data.textures;
        return MeshArgs<with(mesh_mods::JointIndices)>{new_data};
    }

    auto withNormals(std::span<glm::vec3> normals) -> MeshArgs<with(mesh_mods::Normals)>
        requires((Mods & mesh_mods::Normals) == 0)
    {
        MeshData<with(mesh_mods::Normals)> new_data;
        new_data.normals = normals;
        new_data.joint_indices = data.joint_indices;
        new_data.colors = data.colors;
        new_data.vertices = data.vertices;
        new_data.indices = data.indices;
        new_data.textures = data.textures;
        return MeshArgs<with(mesh_mods::Normals)>{new_data};
    }
};