#include <mockstream/models/fixed.h>
#include <mockstream/models/identity.h>
#include <mockstream/stream_point.h>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

#include "utils.h"  // IWYU pragma: keep

using namespace mockstream;

// --------------------
//     UTIL MACROS
// --------------------

/// Indirection macro, so that macro expansion of parameters is done before token pasting.
#define REMOVE_FIRST(...) REMOVE_FIRST_SUB(__VA_ARGS__)

/// Paste all parameters but first.
#define REMOVE_FIRST_SUB(X, ...) __VA_ARGS__

// --------------------
//     Test configs
// --------------------

// -------------------
// ADD HERE
#define MODEL_LIST \
    X(FixedModel)  \
    X(IdentityModel)
// ADD HERE
// -------------------

// configs
#define X(name)                                           \
    struct name##Config {                                 \
        using Model = name;                               \
        using Parameters = typename name::ParametersType; \
        using Estimator = name##Estimator;                \
    };

MODEL_LIST

#undef X

#define X(name) , name##Config

using TestModels = std::tuple<REMOVE_FIRST(MODEL_LIST)>;

#undef X

#undef MODEL_LIST

#undef REMOVE_FIRST_SUB
#undef REMOVE_FIRST

// --------------------
//     Common tests
// --------------------

TEMPLATE_LIST_TEST_CASE("Model generates non-decreasing timestamps", "[model]", TestModels) {
    using M = typename TestType::Model;
    M model;

    auto p1 = model.next();
    auto p2 = model.next();

    REQUIRE(p2.timestamp >= p1.timestamp);
}

TEMPLATE_LIST_TEST_CASE("Estimator processes points and initializes Model", "[model]", TestModels) {
    using M = typename TestType::Model;
    using E = typename TestType::Estimator;
    using P = typename TestType::Parameters;

    // Generate sample data using Model
    M model;
    std::vector<StreamPoint> points;
    for (int i = 0; i < 5; ++i) {
        points.push_back(model.next());
    }

    // Estimate parameters
    E estimator;
    estimator.addAll(points);
    P estimatedParams = estimator.getParams();

    // Verify parameters
    M newModel(estimatedParams);
    REQUIRE(newModel.getParams() == estimatedParams);

    StreamPoint p = newModel.next();
    REQUIRE(p.timestamp >= 0);
}

// ---------------------------
//     IdentityModel tests
// ---------------------------

TEST_CASE("IdentityModel correctness", "[model][identity_model]") {
    SECTION("from zero parameters") {
        IdentityModel model(IdentityModelParameters{});
        REQUIRE(model.getParams().size() == 0);

        for (int i = 0; i < 100; ++i) {
            REQUIRE(model.next() == StreamPoint());
        }
    }

    SECTION("from one parameter") {
        constexpr uint64_t interval = 2;
        constexpr uint64_t amount = 3;
        IdentityModel model(IdentityModelParameters{StreamPoint{2, 3}});
        REQUIRE(model.getParams().size() == 1);

        for (int i = 0; i < 100; ++i) {
            REQUIRE(model.next() == StreamPoint{i * interval, amount});
        }
    }

    SECTION("from several parameters odd end") {
        IdentityModelParameters parameters = {StreamPoint(2, 3), StreamPoint(3, 4), StreamPoint(5, 7)};
        IdentityModel model(parameters);

        // intervals match
        REQUIRE(model.next() == StreamPoint{0, 3});
        REQUIRE(model.next() == StreamPoint{1, 4});
        REQUIRE(model.next() == StreamPoint{3, 7});

        // cycles correctly
        REQUIRE(model.next() == StreamPoint{4, 3});
        REQUIRE(model.next() == StreamPoint{5, 4});
        REQUIRE(model.next() == StreamPoint{7, 7});
    }

    SECTION("from several parameters even end") {
        IdentityModelParameters parameters = {StreamPoint(2, 3), StreamPoint(3, 4), StreamPoint(6, 7)};
        IdentityModel model(parameters);

        // intervals match
        REQUIRE(model.next() == StreamPoint{0, 3});
        REQUIRE(model.next() == StreamPoint{1, 4});
        REQUIRE(model.next() == StreamPoint{4, 7});

        // cycles correctly
        REQUIRE(model.next() == StreamPoint{6, 3});
        REQUIRE(model.next() == StreamPoint{7, 4});
        REQUIRE(model.next() == StreamPoint{10, 7});
    }

    SECTION("estimator working") {
        SECTION("one-by-one") {
            IdentityModelEstimator estimator;

            estimator.add(StreamPoint(0, 0));
            REQUIRE(estimator.getParams() == IdentityModelParameters({{0, 0}}));
            estimator.add(StreamPoint(1, 8));
            REQUIRE(estimator.getParams() == IdentityModelParameters({{0, 0}, {1, 8}}));
            estimator.add(StreamPoint(100, 500));
            REQUIRE(estimator.getParams() == IdentityModelParameters({{0, 0}, {1, 8}, {100, 500}}));
        }

        SECTION("all-together") {
            IdentityModelParameters result = {{1, 2}, {5, 7}, {120, 315}};
            IdentityModelEstimator estimator;

            estimator.addAll(result);
            REQUIRE(estimator.getParams() == result);
        }
    }
}

// ------------------------
//     FixedModel tests
// ------------------------

TEST_CASE("FixedModel generates points with fixed interval and amount", "[model][fixed_model]") {
    FixedModelParameters params{5, 10};
    FixedModel model(params);

    SECTION("First call to next()") {
        auto p1 = model.next();
        CHECK(p1.timestamp == 0);
        CHECK(p1.amount == 10);
    }

    SECTION("Subsequent calls") {
        model.next();  // First call
        auto p2 = model.next();
        CHECK(p2.timestamp == 5);
        CHECK(p2.amount == 10);

        auto p3 = model.next();
        CHECK(p3.timestamp == 10);
        CHECK(p3.amount == 10);
    }
}

TEST_CASE("FixedModelEstimator computes parameters correctly", "[model][fixed_model]") {
    FixedModelEstimator estimator;

    SECTION("Two points with same interval and amount") {
        estimator.add({0, 10});
        estimator.add({5, 10});

        CHECK(estimator.getParams().interval == 5);
        CHECK(estimator.getParams().amount == 10);
    }

    SECTION("Three points with same intervals and amounts") {
        estimator.addAll({{0, 10}, {5, 10}, {10, 10}});

        CHECK(estimator.getParams().interval == 5);
        CHECK(estimator.getParams().amount == 10);
    }

    SECTION("Points with varying intervals and amounts") {
        estimator.add({0, 10});
        estimator.add({3, 15});
        estimator.add({7, 20});

        CHECK(estimator.getParams().interval == 3);  // (3 + 4) / 2 = 3.5 → truncated to 3
        CHECK(estimator.getParams().amount == 15);   // (10 + 15 + 20) / 3 = 15
    }

    SECTION("Only one point") {
        estimator.add({0, 10});

        CHECK(estimator.getParams().interval == 0);  // No intervals computed
        CHECK(estimator.getParams().amount == 10);   // 10 / 1 = 10
    }
}

TEST_CASE("FixedModelEstimator initializes Model correctly", "[fixed_model]") {
    FixedModelEstimator estimator;
    estimator.addAll({{0, 10}, {5, 10}, {10, 10}});

    FixedModelParameters estimatedParams = estimator.getParams();
    REQUIRE(estimatedParams == FixedModelParameters{5, 10});

    FixedModel model(estimatedParams);
    auto p1 = model.next();
    CHECK(p1.timestamp == 0);
    CHECK(p1.amount == 10);

    auto p2 = model.next();
    CHECK(p2.timestamp == 5);
    CHECK(p2.amount == 10);
}