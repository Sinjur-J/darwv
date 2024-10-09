cmake_minimum_required(VERSION 3.22)
project(darwv)

set(CMAKE_CXX_STANDARD 17)

find_package(VTK)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
