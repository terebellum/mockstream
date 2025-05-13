#pragma once

#include <mockstream/stream_point.h>

#include <vector>

namespace mockstream {

/*
    Model Interface.

    Represents mathematical model that generates random stream
*/
class IModel {
public:
    virtual ~IModel() = default;

    /*
        Generates next StreamPoint. It is guaranteed that old.timestamp <= new.timestamp when there
        are no uint64_t overflow
    */
    virtual StreamPoint next() = 0;
};

/*
    Abstract class adding Parameters to model interface
*/
template <typename Parameters>
class ModelBase : public IModel {
protected:
    Parameters parameters;

public:
    using ParametersType = Parameters;

    ModelBase() : parameters() {
    }

    ModelBase(const Parameters &parameters) : parameters(parameters) {
    }

    /*
        Get the Parameters of the model
    */
    [[nodiscard]] const Parameters &getParams() const {
        return parameters;
    }

    [[nodiscard]] Parameters &getParams() {
        return parameters;
    }
};

/*
    Interface of estimator that from stream generates parameters. Can generate it offline and online.
*/
class IModelParamsEstimator {
public:
    virtual ~IModelParamsEstimator() = default;

    /*
       Add StreamPoint to Estimator. Recalulates parameters
    */
    virtual void add(const StreamPoint &point) = 0;

    /*
       Adds all StreamPoints to Estimator. Works no longer than O(params.size() * add()), but can
       be faster.
    */
    virtual void addAll(const std::vector<StreamPoint> &points) {
        for (const auto &point : points) {
            add(point);
        }
    }
};

/*
    Adds Parameters to Estimator interface
*/
template <typename Parameters>
class ModelParamsEstimatorBase : public IModelParamsEstimator {
protected:
    Parameters parameters;

public:
    using ParametersType = Parameters;

    /*
       Gets currents params from Estimator
    */
    [[nodiscard]] const Parameters &getParams() const {
        return parameters;
    }

    [[nodiscard]] Parameters &getParams() {
        return parameters;
    }
};
}  // namespace mockstream