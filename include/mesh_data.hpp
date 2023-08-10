#pragma once

#include <bit>
#include <functional>
#include <span>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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
    };
} // namespace mesh_mods

template<mesh_mods::MeshMods Mods = mesh_mods::None> class MeshArgs
{
    using MeshMods = mesh_mods::MeshMods;

    template<mesh_mods::MeshMods Mod, typename = void> struct ModFieldPointer
    {
        // Unspecialized version is not to be used
        static auto get() { return nullptr; }
    };

#define MAP_MOD_TO_FIELD(MOD, FIELD)                                                               \
    template<typename Dummy> struct ModFieldPointer<MOD, Dummy>                                    \
    {                                                                                              \
        static auto get()                                                                          \
        {                                                                                          \
            return &MeshArgs<Mods>::FIELD;                                                         \
        }                                                                                          \
    };

    MAP_MOD_TO_FIELD(mesh_mods::Color, colors);
    MAP_MOD_TO_FIELD(mesh_mods::Texture, textures);
    MAP_MOD_TO_FIELD(mesh_mods::JointIndices, joint_indices);

#undef MAP_MOD_TO_FIELD
    void assign(auto& to, auto&& what)
    {
        if constexpr (requires { to = std::forward<decltype(what)>(what); })
        {
            to = std::forward<decltype(what)>(what);
        }
    }

    consteval static auto withAdditionally(MeshMods new_mod) -> MeshMods
    {
        return static_cast<MeshMods>(Mods | new_mod);
    }

public:
    template<MeshMods OldMods, typename NewElement>
    MeshArgs(MeshArgs<OldMods> mesh_args, std::span<NewElement> new_data);
    MeshArgs(std::span<glm::vec3> new_positions, std::span<unsigned> new_indices,
             std::span<glm::vec3> new_normals = {}) :
        positions{new_positions}, indices{new_indices}, normals{new_normals} {};

    [[nodiscard]] auto getPositions() const -> auto const& { return positions; }
    [[nodiscard]] auto getIndices() const -> auto const& { return indices; }
    [[nodiscard]] auto getNormals() const -> auto const& { return normals; }
    [[nodiscard]] auto getColors() const -> auto const& { return colors; }
    [[nodiscard]] auto getTextures() const -> auto const& { return textures; }
    [[nodiscard]] auto getJointIndices() const -> auto const& { return joint_indices; }

    [[nodiscard]] constexpr auto numberOfBuffers() const -> std::size_t
    {
        constexpr auto baseline_buffer_count = 3;
        return std::popcount(static_cast<unsigned>(Mods)) + baseline_buffer_count;
    }

    auto withColors(std::span<glm::vec3> new_colors) const
        requires((Mods & mesh_mods::Color) == 0)
    {
        return MeshArgs<withAdditionally(mesh_mods::Color)>{*this, new_colors};
    }

    auto withTextures(std::span<glm::vec2> new_textures) const
        requires((Mods & mesh_mods::Texture) == 0)
    {
        return MeshArgs<withAdditionally(mesh_mods::Texture)>{*this, new_textures};
    }

    auto withJointIndices(std::span<glm::ivec3> new_joint_indices) const
        requires((Mods & mesh_mods::JointIndices) == 0)
    {
        return MeshArgs<withAdditionally(mesh_mods::JointIndices)>{*this, new_joint_indices};
    }

    auto withNormals(std::span<glm::vec3> normals) const { return MeshArgs{*this, normals}; }

private:
    struct None
    {
    };

    template<mesh_mods::MeshMods QueriedMods, typename Type>
    using SupportsOrNone = std::conditional_t<(QueriedMods & Mods) == 0, None, Type>;

    std::span<glm::vec3> positions;
    std::span<unsigned> indices;
    std::span<glm::vec3> normals;
    [[no_unique_address]] SupportsOrNone<mesh_mods::Color, std::span<glm::vec3>> colors;
    [[no_unique_address]] SupportsOrNone<mesh_mods::Texture, std::span<glm::vec2>> textures;
    [[no_unique_address]] SupportsOrNone<mesh_mods::JointIndices, std::span<glm::ivec3>>
        joint_indices;
};

template<mesh_mods::MeshMods Mods>
template<mesh_mods::MeshMods OldMods, typename NewElement>
MeshArgs<Mods>::MeshArgs(MeshArgs<OldMods> old_args, std::span<NewElement> new_data)
{
    assign(positions, old_args.getPositions());
    assign(indices, old_args.getIndices());
    assign(normals, old_args.getNormals());
    assign(colors, old_args.getColors());
    assign(textures, old_args.getTextures());
    assign(joint_indices, old_args.getJointIndices());
    this->*ModFieldPointer<static_cast<MeshMods>(Mods ^ OldMods)>::get() = new_data;
}