//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_DATETIME_UTILS_TEST_H
#define AWMOCK_CORE_DATETIME_UTILS_TEST_H

// Local includes
#include <awsmock/core/DateTimeUtils.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    BOOST_AUTO_TEST_CASE(ConvertToUtcTest) {

        // arrange
        const system_clock::time_point localTime = DateTimeUtils::LocalDateTimeNow();

        // act
        const system_clock::time_point utcTime = DateTimeUtils::ConvertToUtc(localTime);

        // assert
        BOOST_CHECK_EQUAL(7200, std::chrono::duration_cast<std::chrono::seconds>(localTime - utcTime).count());
    }

    BOOST_AUTO_TEST_CASE(UtcOffsetTest) {

        // arrange
        const system_clock::time_point localTime = DateTimeUtils::LocalDateTimeNow();
        const system_clock::time_point utcTime = DateTimeUtils::ConvertToUtc(DateTimeUtils::LocalDateTimeNow());

        // act
        const long offset = DateTimeUtils::UtcOffset();

        // assert
        BOOST_CHECK_EQUAL(offset, std::chrono::duration_cast<std::chrono::seconds>(localTime - utcTime).count());
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_DATETIME_UTILS_TEST_H