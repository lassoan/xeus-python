############################################################################
# Copyright (c) 2017, Sylvain Corlay and Johan Mabille                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

# Unit tests
# ==========

cmake_minimum_required(VERSION 3.1)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(xeus-python-test)

    enable_testing()

    find_package(xeus REQUIRED CONFIG)
    find_package(cppzmq REQUIRED CONFIG)
    find_package(PythonLibs REQUIRED CONFIG)
    find_package(pybind11 REQUIRED CONFIG)
endif ()

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Intel)
    add_compile_options(-Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion)

    CHECK_CXX_COMPILER_FLAG(-march=native HAS_MARCH_NATIVE)
    if (HAS_MARCH_NATIVE)
        add_compile_options(-march=native)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_options(/EHsc /MP /bigobj)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    if(DOWNLOAD_GTEST)
        # Download and unpack googletest at configure time
        configure_file(downloadGTest.cmake.in googletest-download/CMakeLists.txt)
    else()
        # Copy local source of googletest at configure time
        configure_file(copyGTest.cmake.in googletest-download/CMakeLists.txt)
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                     ${CMAKE_CURRENT_BINARY_DIR}/googletest-build)

    set(GTEST_INCLUDE_DIRS "${gtest_SOURCE_DIR}/include")
    set(GTEST_BOTH_LIBRARIES  gtest_main gtest)
else()
    find_package(GTest REQUIRED)
endif()

find_package(Threads)

include_directories(${GTEST_INCLUDE_DIRS} SYSTEM)

set(XEUS_PYTHON_TESTS
    test_xutils.cpp

    ../src/xutils.cpp
)

set(XEUS_PYTHON_TEST_TARGET test_xeus_python)

add_executable(${XEUS_PYTHON_TEST_TARGET} ${XEUS_PYTHON_TESTS})
if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    add_dependencies(${XEUS_PYTHON_TEST_TARGET} gtest_main)
endif()

if (APPLE)
    set_target_properties(${XEUS_PYTHON_TEST_TARGET} PROPERTIES
        MACOSX_RPATH ON
    )
else()
    set_target_properties(${XEUS_PYTHON_TEST_TARGET} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        SKIP_BUILD_RPATH FALSE
    )
endif()

set_target_properties(${XEUS_PYTHON_TEST_TARGET} PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(${XEUS_PYTHON_TEST_TARGET} ${PYTHON_LIBRARIES} xeus ${GTEST_BOTH_LIBRARIES} Threads::Threads)

add_custom_target(xtest COMMAND test_xeus_python DEPENDS ${XEUS_PYTHON_TEST_TARGET})
