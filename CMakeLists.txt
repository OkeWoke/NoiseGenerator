cmake_minimum_required(VERSION 3.21)
project(NoiseProject)
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_CXX_STANDARD 17)

add_executable(NoiseProject main.cpp)

target_link_libraries(NoiseProject ${OpenCV_LIBS})