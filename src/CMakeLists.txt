add_executable(path_tracer)

target_sources(path_tracer
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Bitmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathTracingShader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SkyboxLight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InfinitePlane.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Rectangle.cpp
)

find_package(Threads REQUIRED)

target_include_directories(path_tracer
PRIVATE 
    ${Threads_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/3rdparty
    ${PROJECT_SOURCE_DIR}/include/3rdparty/glm
)

target_link_libraries(path_tracer
PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
)

