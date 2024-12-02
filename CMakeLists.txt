cmake_minimum_required(VERSION 3.0.0)
#project name
project (GeneticOptimizer 
         LANGUAGES CXX)
# Use mpic++ as the compiler
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILER mpic++)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

#Find Python and pybind11
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pybind11 --includes
    OUTPUT_VARIABLE PYBIND11_INCLUDES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Check if pybind11 includes were retrieved successfully
if (PYBIND11_INCLUDES STREQUAL "")
    message(FATAL_ERROR "Failed to retrieve pybind11 includes.")
else ()
    string(REPLACE " " ";" PYBIND11_INCLUDE_LIST ${PYBIND11_INCLUDES})
    include_directories(${PYBIND11_INCLUDE_LIST})
endif ()

execute_process(
    COMMAND python3-config --cflags
    OUTPUT_VARIABLE PYTHON_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Check if pybind11 includes were retrieved successfully
if (PYTHON_CFLAGS STREQUAL "")
    message(FATAL_ERROR "PYTHON_CFLAGS empty")
else ()
    string(REPLACE " " ";" PYTHON_CFLAGS_LIST ${PYBIND11_INCLUDES})
endif ()

execute_process(
    COMMAND python3-config --ldflags
    OUTPUT_VARIABLE PYTHON_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (PYTHON_LDFLAGS STREQUAL "")
    message(FATAL_ERROR "PYTHON_LDFLAGS empty")
else ()
    string(REPLACE " " ";" PYTHON_LDFLAGS_LIST ${PYTHON_LDFLAGS})
endif ()

# Include and link GPI-2
include_directories(/p/scratch/sdlrs/joseph/GPI2-build/include/)
link_directories(/p/scratch/sdlrs/joseph/GPI2-build/lib64/)

# Define the executable and its sources
add_executable(runMe main.cpp)

# Link libraries
target_compile_options(runMe PRIVATE ${PYTHON_CFLAGS_LIST})
target_link_libraries(runMe PRIVATE GPI2 Python3::Python ${PYTHON_LDFLAGS_LIST})

#set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Wno-dev -W -Ofast -lm")
#add_executable(runMe main.cpp)
#target_compile_options(runMe PRIVATE -Wall -Wextra -Wpedantic)
