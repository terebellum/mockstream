cmake_minimum_required(VERSION 3.23)

# ---------------------------------------------------------------------------------------
# Start mockstream project
# ---------------------------------------------------------------------------------------

project(
    mockstream
    VERSION 0.1.1
    LANGUAGES CXX
) 
message(STATUS "Build mockstream ${PROJECT_VERSION}")

# ---------------------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------------------

option(MOCKSTREAM_TEST "Generate the test target" OFF)

# ---------------------------------------------------------------------------------------
# Declare library
# ---------------------------------------------------------------------------------------

add_library(mockstream_mockstream INTERFACE)
add_library(mockstream::mockstream ALIAS mockstream_mockstream)

set_property(
    TARGET mockstream_mockstream PROPERTY
    EXPORT_NAME mockstream 
)

target_include_directories(
    mockstream_mockstream ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(mockstream_mockstream INTERFACE cxx_std_17)

# ---------------------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------------------

include (cmake/install-rules.cmake)

# ---------------------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------------------

if (MOCKSTREAM_TEST)
    include(CTest)
    add_subdirectory(tests)
endif()