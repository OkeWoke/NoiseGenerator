cmake_minimum_required(VERSION 3.21)
project(NoiseProject)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED) #tentative if we even need..


INCLUDE_DIRECTORIES(imgui)
file(GLOB_RECURSE CPP_FILES "imgui/*.cpp")
file(GLOB_RECURSE H_FILES "imgui/*.h")
set(CMAKE_CXX_STANDARD 17)

add_executable(NoiseProject Utility.cpp Utility.h INoiseGen2D.h PerlinNoise2D.h PerlinNoise2D.cpp ValueNoise2D.cpp ValueNoise2D.h main.cpp ${CPP_FILES} ${H_FILES})

target_sources(NoiseProject PRIVATE imgui/imgui.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_demo.cpp)
target_link_libraries(NoiseProject ${OpenCV_LIBS})
target_link_libraries(NoiseProject glfw)
target_link_libraries(NoiseProject OpenGL::GL)