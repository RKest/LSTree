add_library(
        ls_tree
        controler.cpp
        display.cpp
        distribution.cpp
        l_systems.cpp
        mesh.cpp
        random.cpp
        shader.cpp
        shadow_map.cpp
        texture.cpp
        tree.cpp
        joint.cpp
        Ext/obj_loader.cpp
        Ext/stb_image.cpp
        mesh_data.cpp)
target_include_directories(ls_tree PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/Ext
        ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(ls_tree PUBLIC glm::glm OpenGL::GL ${SDL2_LIBRARIES} ${GLEW_LIBRARIES})
target_compile_features(ls_tree PUBLIC cxx_std_20)