cmake_minimum_required(VERSION 3.20)
project(cube_terrain VERSION 1.0.0 LANGUAGES CXX C)

# C++ Standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source Files
set(SOURCES
    src/main.cpp
    src/core/graphics_core.cpp
    src/core/cubo.cpp
    src/render/mesh.cpp
    src/render/shader.cpp
    src/utils/shader_loader.cpp
)

# Create the target FIRST
add_executable(${PROJECT_NAME} ${SOURCES})

# Now it's safe to modify it
if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

# OpenGL (works on all platforms)
find_package(OpenGL REQUIRED)

# GLAD (vendored)
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/glad/include
)

# Platform-specific dependencies
if(WIN32)
    # GLFW (vendored)
    add_subdirectory(libs/glfw)
    set(GLFW_LIB glfw)

    # GLM (header-only, vendored)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/glm
    )

    # Windows system libs
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ws2_32
        gdi32
        user32
        opengl32
    )

else()
    # Linux: use system packages
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)

    set(GLFW_LIB glfw)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        glm::glm
    )
endif()

# Common includes
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/glad/include
)

# Link everything
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    ${GLFW_LIB}
    glad
)

# Copy shaders
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()