//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_CRON_UTILS_TEST_H
#define AWMOCK_CORE_CRON_UTILS_TEST_H

// C++ includes
#include <chrono>

// AwsMock includes
#include <awsmock/core/CronUtils.h>
#include <awsmock/core/DateTimeUtils.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    BOOST_AUTO_TEST_CASE(CronTimestampTest) {

        // arrange

        // act
        const system_clock::time_point datetime = CronUtils::GetNextExecutionTime("0 0 0 * * ?") + std::chrono::hours(2);
        const system_clock::time_point midnight = std::chrono::ceil<std::chrono::days>(system_clock::now());

        // assert
        BOOST_CHECK_EQUAL(datetime, midnight);
    }

    BOOST_AUTO_TEST_CASE(CronSecondsTest) {

        // arrange

        // act
        const long datetimeSeconds = CronUtils::GetNextExecutionTimeSeconds("0 0 0 * * ?") + 7200;
        const system_clock::time_point midnight = std::chrono::ceil<std::chrono::days>(system_clock::now());
        const long seconds = std::chrono::duration_cast<std::chrono::seconds>(midnight - system_clock::now()).count();

        // assert
        BOOST_CHECK_EQUAL(datetimeSeconds, seconds);
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_CRON_UTILS_TEST_H