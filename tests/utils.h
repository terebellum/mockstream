#pragma once

#include <mockstream/stream_point.h>

#include <catch2/catch_tostring.hpp>

/*
    Printing specifications
*/

template <>
struct Catch::StringMaker<mockstream::StreamPoint> {
    static std::string convert(const mockstream::StreamPoint &sp) {
        return "{ " + std::to_string(sp.timestamp) + ", " + std::to_string(sp.amount) + " }";
    }
};