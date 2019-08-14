project(FaceRendererHM)


add_executable(hmrenderer main.cc)
target_sources(hmrenderer 
PUBLIC
    face.h
    face.cc
    ${FaceRenderer_Headers}
)


target_include_directories(hmrenderer 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hmrenderer glm::glm)
target_link_libraries(hmrenderer glad::glad)
target_link_libraries(hmrenderer glfw)
