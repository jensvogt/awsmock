//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_HTTP_UTILS_TEST_H
#define AWMOCK_CORE_HTTP_UTILS_TEST_H

// AwsMock includes
#include <awsmock/core/HttpUtils.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(QueryStringTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?listType=1";

        // act
        const std::string result = HttpUtils::GetQueryString(uri);

        // assert
        BOOST_CHECK_EQUAL(result, "listType=1");
    }

    BOOST_AUTO_TEST_CASE(CountParametersTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?listType=1";

        // act
        const int result = HttpUtils::CountQueryParameters(uri);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(GetPathParameterTest) {

        // arrange
        const std::string uri = "/2017-03-31/tags/arn%3Aaws%3Alambda%3Aeu-central-1%3A000000000000%3Afunction%3Aftp-file-copy";

        // act
        const std::string result = HttpUtils::GetPathParameter(uri, 2);

        // assert
        BOOST_CHECK_EQUAL(result, "arn:aws:lambda:eu-central-1:000000000000:function:ftp-file-copy");
    }

    BOOST_AUTO_TEST_CASE(CountByNameTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const int result = HttpUtils::CountQueryParametersByPrefix(uri, "arg");

        // assert
        BOOST_CHECK_EQUAL(3, result);
    }

    BOOST_AUTO_TEST_CASE(GetParameterValueTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming?arg1=1";

        // act
        const std::string result = HttpUtils::GetStringParameter(uri, "arg1");

        // assert
        BOOST_CHECK_EQUAL("1", result);
    }

    BOOST_AUTO_TEST_CASE(GetPathParametersTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const std::vector<std::string> result = HttpUtils::GetPathParameters(uri);

        // assert
        BOOST_CHECK_EQUAL(3, result.size());
        BOOST_CHECK_EQUAL("ftpuser1", result[0]);
        BOOST_CHECK_EQUAL("incoming", result[1]);
        BOOST_CHECK_EQUAL("mix", result[2]);
    }

    BOOST_AUTO_TEST_CASE(GetPathParametersFromIndexTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix/pim123.xml";

        // act
        const std::string key = HttpUtils::GetPathParameter(uri, 1);

        // assert
        BOOST_CHECK_EQUAL("incoming", key);
    }

    BOOST_AUTO_TEST_CASE(GetQueryParametersByPrefixTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const std::vector<std::string> result = HttpUtils::GetQueryParametersByPrefix(uri, "arg");

        // assert
        BOOST_CHECK_EQUAL(3, result.size());
        BOOST_CHECK_EQUAL("1", result[0]);
        BOOST_CHECK_EQUAL("2", result[1]);
        BOOST_CHECK_EQUAL("3", result[2]);
    }

    BOOST_AUTO_TEST_CASE(GetQueryParameterByPrefixIndexTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg1=1&arg2=2&arg3=3";

        // act
        const std::string result = HttpUtils::GetQueryParameterByPrefix(uri, "arg", 1);

        // assert
        BOOST_CHECK_EQUAL("1", result);
    }

    BOOST_AUTO_TEST_CASE(GetQueryParameterByNameTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=value";

        // act
        const std::string result = HttpUtils::GetStringParameter(uri, "arg");

        // assert
        BOOST_CHECK_EQUAL("value", result);
    }

    BOOST_AUTO_TEST_CASE(HasQueryParameterTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=value";

        // act
        const bool result = HttpUtils::HasQueryParameter(uri, "arg");

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(IsUrlEncodedTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=This%20is%20an%20encoded%20query%20parameter%20mit%20length%20%3E%201";

        // act
        const bool result = HttpUtils::HasQueryParameter(uri, "arg");

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(GetUrlEncodedQueryParameterTest) {

        // arrange
        const std::string uri = "/ftpuser1/incoming/mix?arg=This%20is%20an%20encoded%20query%20parameter%20mit%20length%20%3E%201";

        // act
        const std::string result = HttpUtils::GetStringParameter(uri, "arg");

        // assert
        BOOST_CHECK_EQUAL(result, "This is an encoded query parameter mit length > 1");
    }

    BOOST_AUTO_TEST_CASE(GetVersionActionTest) {

        // arrange
        const std::string uri = "/2016-03-04/functions/functions/invocations";

        // act
        std::string version, action;
        HttpUtils::GetVersionAction(uri, version, action);

        // assert
        BOOST_CHECK_EQUAL(version, "2016-03-04");
        BOOST_CHECK_EQUAL(action, "functions");
    }

    BOOST_AUTO_TEST_CASE(GetStringParameterTest) {

        // arrange
        const std::string uri = "/ftpuser1/testqueue?stringParameter1=testvalue";

        // act
        const int count = HttpUtils::CountQueryParametersByPrefix(uri, "stringParameter");
        const std::string parameter = HttpUtils::GetQueryParameterByPrefix(uri, "stringParameter", 1);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(parameter, "testvalue");
    }

    BOOST_AUTO_TEST_CASE(HasQueryParameterSingle) {

        // arrange
        const std::string uri = "/ftpuser1/testqueue?enabled";

        // act
        const bool enabled = HttpUtils::HasQueryParameter(uri, "enabled");

        // assert
        BOOST_CHECK_EQUAL(true, enabled);
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_HTTP_UTILS_TEST_H