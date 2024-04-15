cmake_minimum_required(VERSION 3.1)

project(VectorAddition LANGUAGES C)

find_package(OpenCL REQUIRED)
find_package(OpenCLHeaders REQUIRED)

add_executable(${PROJECT_NAME} main.c
        kernel.cl
        def.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::Headers)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=100)
configure_file(${PROJECT_SOURCE_DIR}/kernel.cl ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel.cl COPYONLY)