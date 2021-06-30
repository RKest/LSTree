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

#include "_config.h"

#include "display.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "transform.h"
#include "camera.h"
#include "controler.h"
#include "mesh_maker.h"
#include "l_systems.h"
#include "distribution.h"
#include "shadow_map.h"
#include "random.h"

template <class M>
void renderMesh(Shader &shader, M &mesh, Transform &transform, Camera &camera);
template <class M>
void renderMesh(Shader &shader, M &mesh, Transform &transform, Camera &camera, glm::vec3 &lightPos);
template <class M>
void renderMesh(std::vector<std::unique_ptr<M>> &meshes, Shader &shader, Transform &transform, Camera &camera, glm::vec3 &lightPos);
template <class M>
void renderMesh(std::vector<std::unique_ptr<M>> &meshes);

int main(int argc, char **args)
{
    ui noIterations = argc > 1 ? std::atoi(args[1]) : 1;

    CustomRand customRand(69420);
    Display display(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");

    std::vector<std::unique_ptr<AnimatedColouredMesh>> treeMeshes;
    std::vector<std::unique_ptr<MeshMaker>> makers;

    const std::string floorParams[] = {"position", "normal"};
    const std::string lightParams[] = {"position", "normal"};
    const std::string treeParams[] = {"position", "normal", "colour"};
    const std::string animatedTreeParams[] = {"position", "normal", "colour", "jointIndex"};
    const std::string shadowMapParams[] = {"position"};
    const std::string uniforms[] = {"transform", "projection"};

    Shader floorShader("./Shaders/Floor", floorParams, ARR_SIZE(floorParams), uniforms, ARR_SIZE(uniforms));
    Shader lightShader("./Shaders/Light", lightParams, ARR_SIZE(lightParams), uniforms, ARR_SIZE(uniforms));
    Shader treeShader("./Shaders/Tree", treeParams, ARR_SIZE(treeParams), uniforms, ARR_SIZE(uniforms));
    Shader animatedTreeShader("./Shaders/Tree2", animatedTreeParams, ARR_SIZE(animatedTreeParams), uniforms, ARR_SIZE(uniforms));
    Shader shadowMapShader("./Shaders/ShadowMap", shadowMapParams, ARR_SIZE(shadowMapParams), uniforms, ARR_SIZE(uniforms), true);

    const std::vector<MatSeedPair> matSeedPairs = 
        Distribution(customRand, -TREE_AREA_RADIOUS, TREE_AREA_RADIOUS, -TREE_AREA_RADIOUS, TREE_AREA_RADIOUS, TREE_MIN_PROXIMA, TREE_NO).ToMatSeedPair();

    std::for_each(
        std::execution::par,
        matSeedPairs.begin(),
        matSeedPairs.end(),
        [&makers, noIterations](auto &&pair)
        {
            makers.push_back(std::make_unique<MeshMaker>(pair.seed, pair.mat, WALL_NO, TreeLS(pair.seed, noIterations).Result()));
        });

    for (auto &m : makers)
    {
        treeMeshes.push_back(std::move(m->TreeMesh()));
        treeMeshes.push_back(std::move(m->StemMesh()));
        treeMeshes.push_back(std::move(m->BladeMesh()));
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

    float counter = 0.0f;

    float r = 17.0 / 255.0;
    float g = 51.0 / 255.0;
    float b = 72.0 / 255.0;

    Uint64 startPerf, endPerf;

    while (!display.IsClosed())
    {
        startPerf = SDL_GetPerformanceCounter();
        
        display.Clear(r, g, b, 1.0f);

        controler.CaptureKeyboardPresses();
        controler.CaptureMouseMovement();

        lightTransform.Pos().y = sinf(counter) * 2 + 6.0;
        lightTransform.Pos().x = sinf(counter) * 6.0;
        lightTransform.Pos().z = cosf(counter) * 6.0;
        lightTransform.Rot().y = counter;

        shadowMap.Apply(shadowMapShader, blankTransform, lightTransform.Pos());
        renderMesh(treeMeshes);
        shadowMap.CleanUp(SCREEN_WIDTH, SCREEN_HEIGHT);

        renderMesh(floorShader, floor, blankTransform, camera, lightTransform.Pos());
        renderMesh(lightShader, cube, lightTransform, camera);
        renderMesh(treeMeshes, treeShader, blankTransform, camera, lightTransform.Pos());

        display.Update();

        endPerf = SDL_GetPerformanceCounter();

        ft elapsedMS = (endPerf - startPerf) / (ft)SDL_GetPerformanceFrequency() * 1000.0f;
        // std::cout << elapsedMS << std::endl;
        // SDL_Delay(16.666f - elapsedMS);
        counter += 0.01f;
    }

    return 0;
}

template <class M>
void renderMesh(Shader &shader, M &mesh, Transform &transform, Camera &camera)
{
    shader.Bind();
    shader.Update(transform, camera);

    mesh.Draw();
}

template <class M>
void renderMesh(Shader &shader, M &mesh, Transform &transform, Camera &camera, glm::vec3 &lightPos)
{
    shader.Bind();
    shader.Update(transform, camera);

    shader.SetVec3("light.position", lightPos);
    shader.SetVec3("viewPos", camera.Pos());

    shader.SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.SetVec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    shader.SetFloat("light.constant", 1.0f);
    shader.SetFloat("light.linear", 0.9f);
    shader.SetFloat("light.quadratic", 0.032f);

    shader.SetFloat("far_plane", SHADOW_FAR_PLANE);

    mesh.Draw();
}

template <class M>
void renderMesh(std::vector<std::unique_ptr<M>> &meshes, Shader &shader, Transform &transform, Camera &camera, glm::vec3 &lightPos)
{
    shader.Bind();
    shader.Update(transform, camera);

    shader.SetVec3("light.position", lightPos);
    shader.SetVec3("viewPos", camera.Pos());

    shader.SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.SetVec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    shader.SetFloat("light.constant", 1.0f);
    shader.SetFloat("light.linear", 0.9f);
    shader.SetFloat("light.quadratic", 0.032f);

    shader.SetFloat("far_plane", SHADOW_FAR_PLANE);

    for (auto &m : meshes)
        m->Draw();
}

template <class M>
void renderMesh(std::vector<std::unique_ptr<M>> &meshes)
{
    for (auto &m : meshes)
        m->Draw();
}