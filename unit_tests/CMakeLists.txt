cmake_minimum_required(VERSION 3.16)

project(GraphLabUnitTests LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Build unit tests with gcov/lcov coverage instrumentation" OFF)

set(GRAPHLAB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../GraphLab/src")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test)

set(GRAPHLAB_CORE_SOURCES
  "${GRAPHLAB_SRC_DIR}/graph/graph.cpp"
  "${GRAPHLAB_SRC_DIR}/graph/graph.hpp"
  "${GRAPHLAB_SRC_DIR}/graph/node.hpp"
  "${GRAPHLAB_SRC_DIR}/graph/edge.hpp"
  "${GRAPHLAB_SRC_DIR}/Serialization/Serializable.hpp"
  "${GRAPHLAB_SRC_DIR}/Serialization/Serializer.cpp"
  "${GRAPHLAB_SRC_DIR}/Serialization/Serializer.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/algorithmrunnable.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/BFS/bfs.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/BFS/bfs.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/DFS/dfs.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/DFS/dfs.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Dijkstra/dijkstra.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Dijkstra/dijkstra.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Astar/astar.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Astar/astar.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Floyd_Warshall/floyd_warshall.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Floyd_Warshall/floyd_warshall.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/MST_Prim/mst_prim.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/MST_Prim/mst_prim.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/MST_Kruskal/mst_kruskal.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/MST_Kruskal/mst_kruskal.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Euler/euler.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Euler/euler.hpp"
)

add_executable(graphlab_unit_tests
  tests/graphlab_unit_tests.hpp
  tests/test_main.cpp
  tests/graph_unit_tests.cpp
  tests/algorithm_unit_tests.cpp
  tests/serialization_unit_tests.cpp
  ${GRAPHLAB_CORE_SOURCES}
)

target_include_directories(graphlab_unit_tests PRIVATE "${GRAPHLAB_SRC_DIR}")
target_link_libraries(graphlab_unit_tests PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Test)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(graphlab_unit_tests PRIVATE -O0 -g --coverage)
  target_link_options(graphlab_unit_tests PRIVATE --coverage)
endif()

include(CTest)
add_test(NAME graphlab_unit_tests COMMAND graphlab_unit_tests)
