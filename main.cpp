#include <GL/glew.h>
#include "glm/gtx/string_cast.hpp"

#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <execution>
#include <mutex>
#include <cassert>

#include "_config.h"

#include "display.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "transform.h"
#include "camera.h"
#include "controler.h"
#include "tree.h"
#include "l_systems.h"
#include "distribution.h"
#include "shadow_map.h"
#include "random.h"

template <class M>
using meshesPack = std::vector<std::array<std::unique_ptr<M>, 3>>;
using jointsPack = std::vector<std::unique_ptr<std::vector<Joint *>>>;

void bindShader(Shader &shader, const Transform &transform, const Camera &camera);
template <class M>
void drawMeshes(Shader &shader, meshesPack<M> &meshes);

void setLightParams(Shader &shader, const glm::vec3 &lightPos, const glm::vec3 &viewPos);
void alterWindCounter(CustomRand &customRand, ft &windCounter, ui &framesSinceLastGust);
void setJointParams(Shader &shader, const jointsPack &joints, ft windCounter, CustomRand &r);

int main(int argc, char **args)
{
    glm::mat4 r = glm::rotate(glm::radians(90.0f), glm::vec3(0,1,0));
    glm::mat4 t = glm::translate(glm::vec3(3,4,7));

    printf("%s\n", glm::to_string(r).c_str());
    printf("%s\n", glm::to_string(t).c_str());
    printf("%s\n", glm::to_string(r * t).c_str());
    printf("%s\n", glm::to_string(t * r).c_str());

    ui noIterations = argc > 1 ? std::atoi(args[1]) : 1;

    CustomRand customRand(69420);
    Display display(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");

    meshesPack<AnimatedColouredMesh> treeMeshes;
    jointsPack joints;
    std::vector<std::unique_ptr<Tree>> trees;

    const std::string floorParams[] = {"position", "normal"};
    const std::string lightParams[] = {"position", "normal"};
    const std::string treeParams[] = {"position", "normal", "colour"};
    const std::string animatedTreeParams[] = {"position", "jointIndices", "normal", "colour"};
    const std::string shadowMapParams[] = {"position", "jointIndices"};
    const std::string uniforms[] = {"transform", "projection"};

    Shader floorShader("./Shaders/Floor", floorParams, ARR_SIZE(floorParams), uniforms, ARR_SIZE(uniforms));
    Shader lightShader("./Shaders/Light", lightParams, ARR_SIZE(lightParams), uniforms, ARR_SIZE(uniforms));
    Shader treeShader("./Shaders/Tree", treeParams, ARR_SIZE(treeParams), uniforms, ARR_SIZE(uniforms));
    Shader animatedTreeShader("./Shaders/Tree2", animatedTreeParams, ARR_SIZE(animatedTreeParams), uniforms, ARR_SIZE(uniforms));
    Shader shadowMapShader("./Shaders/ShadowMap", shadowMapParams, ARR_SIZE(shadowMapParams), uniforms, ARR_SIZE(uniforms), true);

    const std::vector<MatSeedPair> matSeedPairs =
        Distribution(customRand, -TREE_AREA_RADIOUS, TREE_AREA_RADIOUS, -TREE_AREA_RADIOUS, TREE_AREA_RADIOUS, TREE_MIN_PROXIMA, TREE_NO).ToMatSeedPair();

    std::for_each(
        std::execution::par_unseq,
        matSeedPairs.begin(),
        matSeedPairs.end(),
        [&trees, noIterations](auto &&pair)
        {
            Joint *rootJointPtr = new Joint(pair.mat, 0, nullptr);
            CustomRand rand(pair.seed);
            trees.push_back(std::make_unique<Tree>(pair.mat, WALL_NO, TreeLS(pair.seed, noIterations).Result(), rand, rootJointPtr));
        });

    for (auto &t : trees)
    {
        treeMeshes.push_back({std::move(t->BarkMesh()),
                              std::move(t->BladeMesh()),
                              std::move(t->StemMesh())});

        joints.push_back(std::move(t->JointPtrVectorPtr()));
    }

    ShadowMap shadowMap;

    treeShader.Bind();
    treeShader.SetInt("depthMap", 0);

    floorShader.Bind();
    floorShader.SetInt("depthMap", 0);

    glm::vec3 floorVertices[] = {
        glm::vec3(-100, -1, -100),
        glm::vec3(-100, -1, 100),
        glm::vec3(100, -1, 100),
        glm::vec3(100, -1, -100)};

    ui floorIndicies[] = {0, 1, 2, 3, 0, 2};

    UntexturedMesh floor(floorVertices, ARR_SIZE(floorVertices), floorIndicies, ARR_SIZE(floorIndicies));
    UntexturedMesh cube("./Resources/obj/cube.obj", false);

    Transform blankTransform;
    Transform lightTransform;

    lightTransform.Scale() = glm::vec3(0.1, 0.1, 0.1);

    Camera camera(glm::vec3(0, 0, 5), 70.0f, display.Aspect(), 0.01f, 1000.0f);
    Controler controler(display, camera);

    ft counter = 0.0f;
    ft windCounter = 0.0f;
    ui framesSinceLastGust = 0;

    while (!display.IsClosed())
    {
        display.Clear(0.066f, 0.2f, 0.282f, 1.0f);

        //Input
        controler.CaptureKeyboardPresses();
        controler.CaptureMouseMovement();

        //Setting the position of the light source (cube)
        lightTransform.Pos().y = sinf(counter) * 2 + 6.0;
        lightTransform.Pos().x = sinf(counter) * 6.0;
        lightTransform.Pos().z = cosf(counter) * 6.0;
        lightTransform.Rot().y = counter;

        //Loading the shadow map
        shadowMap.Apply(shadowMapShader, blankTransform, lightTransform.Pos());
        setJointParams(shadowMapShader, joints, windCounter, customRand);
        drawMeshes(shadowMapShader, treeMeshes);
        shadowMap.CleanUp(SCREEN_WIDTH, SCREEN_HEIGHT);

        //Drawing the floor
        bindShader(floorShader, blankTransform, camera);
        setLightParams(floorShader, lightTransform.Pos(), camera.Pos());
        floor.Draw();

        //Drawing the light source (cube)
        bindShader(lightShader, lightTransform, camera);
        cube.Draw();

        //Drawing the trees
        bindShader(animatedTreeShader, blankTransform, camera);
        setLightParams(animatedTreeShader, lightTransform.Pos(), camera.Pos());
        setJointParams(animatedTreeShader, joints, windCounter, customRand);
        drawMeshes(animatedTreeShader, treeMeshes);

        display.Update();
        alterWindCounter(customRand, windCounter, framesSinceLastGust);
        counter += 0.01f;
    }

    return 0;
}

void bindShader(Shader &shader, const Transform &transform, const Camera &camera)
{
    shader.Bind();
    shader.Update(transform, camera);
    shader.SetFloat("far_plane", SHADOW_FAR_PLANE);
}

template <class M>
void drawMeshes(Shader &shader, meshesPack<M> &meshes)
{
    for (ui i = 0; i < meshes.size(); ++i)
    {
        shader.SetInt("currTreeIndex", i);
        for (auto &mesh : meshes[i])
            mesh->Draw();
    }
}

void setLightParams(Shader &shader, const glm::vec3 &lightPos, const glm::vec3 &viewPos)
{
    shader.SetVec3("light.position", lightPos);
    shader.SetVec3("viewPos", viewPos);

    shader.SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.SetVec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    shader.SetFloat("light.constant", 1.0f);
    shader.SetFloat("light.linear", 0.9f);
    shader.SetFloat("light.quadratic", 0.032f);
}

void alterWindCounter(CustomRand &customRand, ft &windCounter, ui &framesSinceLastGust)
{
    if (customRand.NextFloat(0.0f, 1.0f) < WIND_GUST_ODDS)
    {
        std::cout << "Phew :3" << std::endl;
        framesSinceLastGust = 0;
    }
    else
    {
        ++framesSinceLastGust;
        windCounter += WIND_GUST_STRENGTH * pow(WIND_GUST_DECAY_RATE, abs(-((int)framesSinceLastGust) + 15) / 9) * (WIND_MAX_WIND_COUNTER - windCounter);
        windCounter *= WIND_DECAY_RATE;
    }

    if (windCounter > 1.0f)
        windCounter = 1.0f;
}

void setJointParams(Shader &shader, const jointsPack &joints, ft windCounter, CustomRand &r)
{
    assert(joints[0]->size() <= MAX_JOINT_AMOUNT);

    shader.SetFloat("windAffectionAngle", glm::radians(WIND_AFFECTION_ANGLE));
    shader.SetVec3("windDirectionVector", WIND_DIRECTION_VECTOR);
    shader.SetFloat("windStrenght", windCounter);

    for (ui i = 0; i < joints.size(); ++i)
        for (ui j = 0; j < joints[0]->size(); ++j)
        {
            shader.SetMat4("jointBaseTransforms[" + std::to_string(i) + "][" + std::to_string(j) + "]", (*joints[i])[j]->baseTransform);
            shader.SetVec3("jointVectors[" + std::to_string(i) + "][" + std::to_string(j) + "]", (*joints[i])[j]->toParentVector);
        }
}