#pragma once

#include <mockstream/model.h>

#include <cstdint>

namespace mockstream {

/*
    Parameters used in fixed ModelBase

    mean_interval - mean interval of all stream data
    mean_amount - mean amount interval data

    O(1) memory
*/
struct FixedModelParameters {
    uint64_t interval = 0;
    uint64_t amount = 0;

    bool operator==(const FixedModelParameters &other) const {
        return interval == other.interval && amount == other.amount;
    }
};

/*
    Returns always same amount with same passing time
*/
class FixedModel final : public ModelBase<FixedModelParameters> {
private:
    uint64_t last_time = 0;

public:
    using ModelBase<FixedModelParameters>::ModelBase;

    /*
        O(1)
    */
    StreamPoint next() final {
        uint64_t return_val = last_time;
        last_time += parameters.interval;
        return {return_val, parameters.amount};
    }
};

/*
    Keeps sums of both metrics to return their means later
*/
class FixedModelEstimator final : public ModelParamsEstimatorBase<FixedModelParameters> {
private:
    uint64_t sum_time = 0;
    uint64_t last_timestamp = 0;
    uint64_t sum_amount = 0;
    uint64_t count = 0;

public:
    /*
        O(1)
    */
    void add(const StreamPoint &point) final {
        // Add to sum_time and sum_amount
        if (!count) {
            sum_time += point.timestamp;
        } else {
            sum_time += point.timestamp - last_timestamp;
        }
        sum_amount += point.amount;
        ++count;

        // Update timestamp and count
        last_timestamp = point.timestamp;

        // Update means
        parameters.interval = count <= 1 ? sum_time : sum_time / (count - 1);
        parameters.amount = count == 0 ? sum_amount : sum_amount / count;
    }
};

}  // namespace mockstream