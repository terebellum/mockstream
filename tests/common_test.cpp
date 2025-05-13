#include <mockstream/common.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("is_unique checks for unique types in a parameter pack", "[is_unique]") {
    SECTION("Empty pack is unique") {
        REQUIRE(mockstream::is_unique<> == true);
    }

    SECTION("Single type is unique") {
        REQUIRE(mockstream::is_unique<int> == true);
    }

    SECTION("Two different types are unique") {
        REQUIRE(mockstream::is_unique<int, float> == true);
    }

    SECTION("Two identical types are not unique") {
        REQUIRE(mockstream::is_unique<int, int> == false);
    }

    SECTION("Three unique types") {
        REQUIRE(mockstream::is_unique<int, float, char> == true);
    }

    SECTION("Duplicate at the end") {
        REQUIRE(mockstream::is_unique<int, float, int> == false);
    }

    SECTION("Duplicate in the middle") {
        REQUIRE(mockstream::is_unique<float, int, int, char> == false);
    }

    SECTION("Different cv-qualified and pointer types are unique") {
        REQUIRE(mockstream::is_unique<int, const int, volatile int, int *> == true);
    }

    SECTION("Large pack with all unique types") {
        REQUIRE(mockstream::is_unique<void, char, short, int, long> == true);
    }

    SECTION("Large pack with a duplicate") {
        REQUIRE(mockstream::is_unique<char, int, double, int, float> == false);
    }
}