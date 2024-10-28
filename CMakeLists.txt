cmake_minimum_required(VERSION 3.0.0)
#project name
project (GeneticOptimizer 
         LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Wno-dev -W -Ofast -lm")
add_executable(runMe main.cpp)
target_compile_options(runMe PRIVATE -Wall -Wextra -Wpedantic)
