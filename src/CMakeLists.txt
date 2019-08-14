cmake_minimum_required(VERSION 3.9)

project(FaceRendererHM)

get_filename_component(ROOT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm REQUIRED)

include_directories(includes)
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE FaceRenderer_Headers 
    "depvars.h" 
    "includes/*.h" 
    "includes/*.cc"
    "includes/*.cpp"
)

add_subdirectory(renderer_hm)

