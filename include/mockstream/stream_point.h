#pragma once

#include <cstddef>
#include <cstdint>
#include <ctime>

namespace mockstream {

/*
   Class representing one Point of timestamp, used to generate parameters for generators
*/
struct StreamPoint {
    uint64_t timestamp = 0;
    uint64_t amount = 0;

    StreamPoint() = default;

    StreamPoint(uint64_t timestamp, uint64_t amount) : timestamp(timestamp), amount(amount) {
    }

    bool operator==(const StreamPoint &other) const {
        return timestamp == other.timestamp && amount == other.amount;
    }
};

}  // namespace mockstream
