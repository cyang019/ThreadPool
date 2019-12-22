
add_library(lean INTERFACE)

target_sources(lean INTERFACE threadpool.h)

add_executable(example example.cpp)

