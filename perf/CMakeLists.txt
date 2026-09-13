cmake_minimum_required(VERSION 3.16)

project(GraphLabPerfBenchmark LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAPHLAB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../GraphLab/src")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(graphlab_perf_benchmark
  benchmark.cpp
  "${GRAPHLAB_SRC_DIR}/graph/graph.cpp"
  "${GRAPHLAB_SRC_DIR}/graph/graph.hpp"
  "${GRAPHLAB_SRC_DIR}/graph/node.hpp"
  "${GRAPHLAB_SRC_DIR}/graph/edge.hpp"
  "${GRAPHLAB_SRC_DIR}/Serialization/Serializable.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/algorithmrunnable.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Dijkstra/dijkstra.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Dijkstra/dijkstra.hpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Floyd_Warshall/floyd_warshall.cpp"
  "${GRAPHLAB_SRC_DIR}/algorithm/Floyd_Warshall/floyd_warshall.hpp"
)

target_include_directories(graphlab_perf_benchmark PRIVATE "${GRAPHLAB_SRC_DIR}")
target_link_libraries(graphlab_perf_benchmark PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_compile_options(graphlab_perf_benchmark PRIVATE -fno-omit-frame-pointer)
