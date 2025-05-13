#pragma once

#include <mockstream/model.h>
#include <mockstream/stream_point.h>

#include <vector>

namespace mockstream {

/*
   It is just vector of stream points

   O(n) memory
*/
using IdentityModelParameters = std::vector<StreamPoint>;

/*
   ModelBase that returns points with same values and intervals that were given to
   parameters in a cycle. Starts with 0.
*/
class IdentityModel final : public ModelBase<IdentityModelParameters> {
private:
    uint64_t last_time = 0;
    size_t current_id = 0;

public:
    using ModelBase<IdentityModelParameters>::ModelBase;

    /*
        If parameters have size 0, returns {0,0} If parameters have size 1, goes once in parameters[0].timestamp.

        Between last and first point interval is mean of interavls [t_(n-1), t_(n)] and [t_0, t_1].

        O(1)
    */
    StreamPoint next() final {
        if (parameters.size() == 0) {
            return StreamPoint();
        }
        if (parameters.size() == 1) {
            StreamPoint result = {last_time, parameters.front().amount};
            last_time += parameters.front().timestamp;
            return result;
        }

        current_id = current_id % parameters.size();
        StreamPoint result = {last_time, parameters[current_id].amount};
        if (current_id + 1 == parameters.size()) {
            last_time += (parameters[parameters.size() - 1].timestamp - parameters[parameters.size() - 2].timestamp +
                          parameters[1].timestamp - parameters[0].timestamp) /
                         2;
        } else {
            last_time += parameters[current_id + 1].timestamp - parameters[current_id].timestamp;
        }

        ++current_id;

        return result;
    }
};

/*
    Just stores points into vector.
*/
class IdentityModelEstimator final : public ModelParamsEstimatorBase<IdentityModelParameters> {
public:
    /*
        O(1)
    */
    void add(const StreamPoint &point) final {
        parameters.push_back(point);
    }

    /*
        O(n)
    */
    void addAll(const std::vector<StreamPoint> &points) final {
        parameters.reserve(points.size());
        for (const auto &point : points) {
            parameters.push_back(point);
        }
    }
};

}  // namespace mockstream