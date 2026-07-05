//
// Created by vogje01 on 02/06/2023.
//

// Boost includes
#include <boost/test/unit_test.hpp>

// Awsmock includes
#include <awsmock/core/RandomUtils.h>

namespace Awsmock::Core {

    BOOST_AUTO_TEST_CASE(MinMaxIntTest) {

        // arrange
        constexpr int min = 32768;
        constexpr int max = 65536;

        // act
        std::vector<int> result;
        result.reserve(200);
        for (int i = 0; i < 100; i++) {
            result.emplace_back(RandomUtils::NextInt(min, max));
        }

        // assert
        for (int i = 0; i < 100; i++) {
            BOOST_CHECK(result[i] >= min && result[i] <= max);
        }
        result.clear();
    }

    BOOST_AUTO_TEST_CASE(IntArrayTest) {

        // arrange
        constexpr int min = 32768;
        constexpr int max = 65536;
        constexpr int size = 100;

        // act
        std::vector<int> result = RandomUtils::IntArray(size, min, max);

        // assert
        for (int i = 0; i < size; i++) {
            BOOST_CHECK(result[i] >= min && result[i] <= max);
        }
        result.clear();
    }

    BOOST_AUTO_TEST_CASE(MinMaxDoubleTest) {

        // arrange
        constexpr double min = 0.0;
        constexpr double max = 1.0;

        // act
        std::vector<double> result;
        result.reserve(100);
        for (int i = 0; i < 100; i++) {
            result.emplace_back(RandomUtils::NextDouble(min, max));
        }

        // assert
        for (int i = 0; i < 100; i++) {
            BOOST_CHECK(result[i] > min && result[i] < max);
        }
        result.clear();
    }

    BOOST_AUTO_TEST_CASE(DoubleArrayTest) {

        // arrange
        constexpr double min = 0.0;
        constexpr double max = 1.0;
        constexpr int size = 100;

        // act
        std::vector<double> result = RandomUtils::DoubleArray(size, min, max);

        // assert
        for (int i = 0; i < size; i++) {
            BOOST_CHECK(result[i] > min && result[i] < max);
        }
        result.clear();
    }
}// namespace Awsmock::Core
