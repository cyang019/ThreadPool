cmake_minimum_required (VERSION 3.14)
project (lean 
  LANGUAGES CXX
  VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(lean INTERFACE)

target_sources(lean INTERFACE ${PROJECT_SOURCE_DIR}/threadpool.h)

add_executable(example ${PROJECT_SOURCE_DIR}/example.cpp)


