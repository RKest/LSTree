#pragma once

#include "mesh.h"

#include <filesystem>

#define STATIC_IF(CONDITION, THEN, ELSE)                                                           \
    [&]                                                                                            \
    {                                                                                              \
        if constexpr (CONDITION)                                                                   \
            return THEN;                                                                           \
        else                                                                                       \
            return ELSE;                                                                           \
    }()

struct MeshLoader
{
    template<mesh_mods::MeshMods Mods> static auto load(std::filesystem::path path) -> Mesh
    {
        OBJModel model(path);
        auto indexed_model = model.ToIndexedModel();
        if (Mods & mesh_mods::Normals && indexed_model.normals.empty())
        {
            indexed_model.CalcNormals();
        }
        MeshArgs args{indexed_model.positions, indexed_model.indices};
        const auto& args2 =
            STATIC_IF(Mods & mesh_mods::Normals, args.withNormals(indexed_model.normals), args);
        const auto& args3 =
            STATIC_IF(Mods & mesh_mods::Texture, args2.withTextures(indexed_model.texCoords), args2);
        return Mesh{args3.getData()};
    }
};

#undef STATIC_IF