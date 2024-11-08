// C++ program to create target string, starting from
// random string using Genetic Algorithm

#include <bits/stdc++.h>
#include <bitset>
#include <random>
#include <execution>

//#include "Fitness.cpp"
//#include "Util.cpp"

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <bitset>
#include <vector>

namespace py = pybind11;

template<typename T>
T call_python_function(const std::vector<uint32_t>& bitset_input) {
//    py::scoped_interpreter guard{};  // Initialize the Python interpreter

    py::array_t<uint32_t> bit_array(bitset_input.size(), bitset_input.data());

    // Add custom path to sys.path
    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("append")("/home/joseph/Amer");

    // Load Python function
    py::object my_module = py::module_::import("utils");
    py::object my_function = my_module.attr("cost");

    const auto problem_id = 1;

    // Call Python function and return result as float
    py::object result = my_function(bit_array, INDIVIDUAL_SIZE, problem_id);

    return result.cast<T>();
}

#if ENABLE_TEST
auto main() -> int {

    std::bitset<INDIVIDUAL_SIZE> bitset_input;

    for(auto i=0; i < INDIVIDUAL_SIZE;i++)
        bitset_input[i] = i % 2;

    float result = call_python_function<float>(bitset_input);

    std::cout<<"Result is "<<result<<std::endl;
} 
#endif
