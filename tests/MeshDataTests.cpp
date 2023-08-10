#include "mesh_data.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

MATCHER_P(SpanEqual, span, "")
{
    return span.size() == arg.size() && std::ranges::equal(span, arg);
}

TEST(MeshDataTest, ShouldComposeMeshData)
{
    std::vector<glm::vec3> example_vertices{{1, 2, 3}};
    std::vector<unsigned> example_indices{4};
    std::vector<glm::vec3> example_colors{{5, 6, 7}};
    std::vector<glm::ivec3> example_joint_indices{{8, 9, 10}};
    std::vector<glm::vec2> example_textures{{11, 12}};
    std::vector<glm::vec3> example_normals{{13, 14, 15}};

    auto args = MeshArgs{example_vertices, example_indices};
    EXPECT_THAT(args.getData().vertices, SpanEqual(example_vertices));
    EXPECT_THAT(args.getData().indices, SpanEqual(example_indices));
    EXPECT_EQ(args.getData().getNumberOfBuffers(), 2);

    auto args2 = args.withColors(example_colors);
    EXPECT_THAT(args2.getData().vertices, SpanEqual(example_vertices));
    EXPECT_THAT(args2.getData().indices, SpanEqual(example_indices));
    EXPECT_THAT(args2.getData().colors, SpanEqual(example_colors));
    EXPECT_EQ(args2.getData().getNumberOfBuffers(), 3);

    auto args3 = args2.withTextures(example_textures);
    EXPECT_THAT(args3.getData().vertices, SpanEqual(example_vertices));
    EXPECT_THAT(args3.getData().indices, SpanEqual(example_indices));
    EXPECT_THAT(args3.getData().colors, SpanEqual(example_colors));
    EXPECT_THAT(args3.getData().textures, SpanEqual(example_textures));
    EXPECT_EQ(args3.getData().getNumberOfBuffers(), 4);

    auto args4 = args3.withJointIndices(example_joint_indices);
    EXPECT_THAT(args4.getData().vertices, SpanEqual(example_vertices));
    EXPECT_THAT(args4.getData().indices, SpanEqual(example_indices));
    EXPECT_THAT(args4.getData().colors, SpanEqual(example_colors));
    EXPECT_THAT(args4.getData().textures, SpanEqual(example_textures));
    EXPECT_THAT(args4.getData().joint_indices, SpanEqual(example_joint_indices));
    EXPECT_EQ(args4.getData().getNumberOfBuffers(), 5);

    auto args5 = args4.withNormals(example_normals);
    EXPECT_THAT(args5.getData().vertices, SpanEqual(example_vertices));
    EXPECT_THAT(args5.getData().indices, SpanEqual(example_indices));
    EXPECT_THAT(args5.getData().colors, SpanEqual(example_colors));
    EXPECT_THAT(args5.getData().textures, SpanEqual(example_textures));
    EXPECT_THAT(args5.getData().joint_indices, SpanEqual(example_joint_indices));
    EXPECT_THAT(args5.getData().normals, SpanEqual(example_normals));
    EXPECT_EQ(args5.getData().getNumberOfBuffers(), 6);
}