add_executable(tests
        MeshDataTests.cpp
        )

target_link_libraries(tests PRIVATE ls_tree)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock)
target_compile_features(tests PRIVATE cxx_std_20)
gtest_discover_tests(tests)