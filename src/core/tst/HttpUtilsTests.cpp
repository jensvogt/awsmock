//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_HTTP_UTILS_TEST_H
#define AWMOCK_CORE_HTTP_UTILS_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>

namespace AwsMock::Core {

    class HttpUtilsTest : public ::testing::Test {};

    TEST_F(HttpUtilsTest, QueryStringTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?listType=1";

        // act
        const std::string result = HttpUtils::GetQueryString(uri);

        // assert
        EXPECT_TRUE(result == "listType=1");
    }

    TEST_F(HttpUtilsTest, CountParametersTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?listType=1";

        // act
        const int result = HttpUtils::CountQueryParameters(uri);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(HttpUtilsTest, GetPathParameterTest) {

        // arrange
        const std::string uri = "/2017-03-31/tags/arn%3Aaws%3Alambda%3Aeu-central-1%3A000000000000%3Afunction%3Aftp-file-copy";

        // act
        const std::string result = HttpUtils::GetPathParameter(uri, 2);

        // assert
        EXPECT_TRUE(result == "arn:aws:lambda:eu-central-1:000000000000:function:ftp-file-copy");
    }

    TEST_F(HttpUtilsTest, CountByNameTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const int result = HttpUtils::CountQueryParametersByPrefix(uri, "arg");

        // assert
        EXPECT_EQ(3, result);
    }

    TEST_F(HttpUtilsTest, GetParameterValueTest) {

        // arrange
        const std::string uri = "arg1=1";

        // act
        const std::string result = HttpUtils::GetStringParameter(uri, "arg1");

        // assert
        EXPECT_EQ("1", result);
    }

    TEST_F(HttpUtilsTest, GetPathParametersTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const std::vector<std::string> result = HttpUtils::GetPathParameters(uri);

        // assert
        EXPECT_EQ(3, result.size());
        EXPECT_TRUE("ftpuser1" == result[0]);
        EXPECT_TRUE("incoming" == result[1]);
        EXPECT_TRUE("mix" == result[2]);
    }

    TEST_F(HttpUtilsTest, GetPathParametersFromIndexTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix/pim123.xml";

        // act
        const std::string key = HttpUtils::GetPathParameter(uri, 1);

        // assert
        EXPECT_TRUE("incoming/mix/pim123.xml" == key);
    }

    TEST_F(HttpUtilsTest, GetQueryParametersByPrefixTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const std::vector<std::string> result = HttpUtils::GetQueryParametersByPrefix(uri, "arg");

        // assert
        EXPECT_EQ(3, result.size());
        EXPECT_TRUE("1" == result[0]);
        EXPECT_TRUE("2" == result[1]);
        EXPECT_TRUE("3" == result[2]);
    }

    TEST_F(HttpUtilsTest, GetQueryParameterByPrefixIndexTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const std::string result = HttpUtils::GetQueryParameterByPrefix(uri, "arg", 1);

        // assert
        EXPECT_TRUE("1" == result);
    }

    TEST_F(HttpUtilsTest, GetQueryParameterByNameTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=value";

        // act
        const std::string result = HttpUtils::GetStringParameter(uri, "arg");

        // assert
        EXPECT_TRUE("value" == result);
    }

    TEST_F(HttpUtilsTest, HasQueryParameterTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=value";

        // act
        const bool result = HttpUtils::HasQueryParameter(uri, "arg");

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(HttpUtilsTest, IsUrlEncodedTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=This%20is%20an%20encoded%20query%20parameter%20mit%20length%20%3E%201";

        // act
        const bool result = HttpUtils::HasQueryParameter(uri, "arg");

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(HttpUtilsTest, GetUrlEncodedQueryParameterTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=This%20is%20an%20encoded%20query%20parameter%20mit%20length%20%3E%201";

        // act
        const std::string result = HttpUtils::GetStringParameter(uri, "arg");

        // assert
        EXPECT_TRUE(result == "This is an encoded query parameter mit length > 1");
    }

    TEST_F(HttpUtilsTest, GetVersionActionTest) {

        // arrange
        const std::string uri = "/2016-03-04/functions/functions/invocations";

        // act
        std::string version, action;
        HttpUtils::GetVersionAction(uri, version, action);

        // assert
        EXPECT_TRUE(version == "2016-03-04");
        EXPECT_TRUE(action == "functions");
    }

    TEST_F(HttpUtilsTest, GetStringParameterTest) {

        // arrange
        const std::string uri = "testqueue?stringParameter1=testvalue";

        // act
        const int count = HttpUtils::CountQueryParametersByPrefix(uri, "stringParameter");
        const std::string parameter = HttpUtils::GetQueryParameterByPrefix(uri, "stringParameter", 1);

        // assert
        EXPECT_EQ(1, count);
        EXPECT_TRUE(parameter == "testvalue");
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_HTTP_UTILS_TEST_H