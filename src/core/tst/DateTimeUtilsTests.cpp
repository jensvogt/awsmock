//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_DATETIME_UTILS_TEST_H
#define AWMOCK_CORE_DATETIME_UTILS_TEST_H

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/DateTimeUtils.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    class DateTimeUtilsTest : public ::testing::Test {

      public:

        static system_clock::time_point createDateTime() {
            return DateTimeUtils::LocalDateTimeNow();
        }
    };

    TEST_F(DateTimeUtilsTest, ConvertToUtcTest) {

        // arrange
        const system_clock::time_point localTime = createDateTime();

        // act
        const system_clock::time_point utcTime = DateTimeUtils::ConvertToUtc(localTime);

        // assert
        EXPECT_EQ(3600, std::chrono::duration_cast<std::chrono::seconds>(localTime - utcTime).count());
    }

    TEST_F(DateTimeUtilsTest, UtcOffsetTest) {

        // arrange
        const system_clock::time_point localTime = createDateTime();
        const system_clock::time_point utcTime = DateTimeUtils::ConvertToUtc(createDateTime());

        // act
        long offset = DateTimeUtils::UtcOffset();

        // assert
        EXPECT_EQ(offset, std::chrono::duration_cast<std::chrono::seconds>(localTime - utcTime).count());
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_DATETIME_UTILS_TEST_H