cmake_minimum_required(VERSION 3.16)

# UTF-8 source
add_compile_options("$<$<AND:$<C_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C>>:/utf-8>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CXX>>:/utf-8>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CXX>>:/Zc:__cplusplus>")

# MSVC static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(PROJECT_NAME json)
set(PROJECT_VERSION 1.00)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

if(NOT TARGET tp_stub)
add_subdirectory(../tp_stub ${CMAKE_CURRENT_BINARY_DIR}/tp_stub)
endif()

add_library(${PROJECT_NAME} SHARED
	Main.cpp
)

# target_include_directories(${PROJECT_NAME} PUBLIC
# 	../../../core/base
# )

target_link_libraries(${PROJECT_NAME} PUBLIC
    tp_stub
)
