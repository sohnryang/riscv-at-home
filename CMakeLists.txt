cmake_minimum_required(VERSION 3.19)
project(riscv-at-home)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
find_package(verilator)
add_subdirectory(test)
add_subdirectory(src)
