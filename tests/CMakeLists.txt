cmake_minimum_required(VERSION 3.23)
# ---------------------------------------------------------------------------------------
# Test project start
# ---------------------------------------------------------------------------------------

project(mockstream_tests CXX)

if(NOT TARGET mockstream_mockstream)
    # Stand-alone build
    find_package(mockstream REQUIRED)
    enable_testing()
endif()


# ---------------------------------------------------------------------------------------
# Install Catch2
# ---------------------------------------------------------------------------------------

find_package(Catch2 3 QUIET)
if(Catch2_FOUND)
    message(STATUS "Packaged version of Catch will be used.")
else()
    message(STATUS "Bundled version of Catch will be downloaded and used.")
    include(FetchContent)
    FetchContent_Declare(
        Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG 53d0d913a422d356b23dd927547febdf69ee9081 # v3.5.0
    )
    FetchContent_MakeAvailable(Catch2)
endif()



# ---------------------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------------------

set(MOCKSTREAM_TESTS_SOURCES main.cpp common_test.cpp model_test.cpp)

add_executable(mockstream_test ${MOCKSTREAM_TESTS_SOURCES})
target_link_libraries(mockstream_test PRIVATE mockstream::mockstream Catch2::Catch2WithMain)
target_compile_features(mockstream_test PRIVATE cxx_std_17)

add_test(NAME mockstream_test COMMAND mockstream_test)