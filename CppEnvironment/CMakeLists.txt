cmake_minimum_required(VERSION 3.25)
project(CppEnvironment)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Target 1: Analysis Engine
add_executable(AnalyzisEngine derivativesEngine/ComputeFunction.cpp
        derivativesEngine/DifferentiationEngine.h)

# Target 2: Matrix Determinant Calculator
add_executable(MatrixDetCalc resources/MatrixDetCalc.cpp)
