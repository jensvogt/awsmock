//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_CORE_STRING_UTILS_TEST_H
#define AWSMOCK_CORE_STRING_UTILS_TEST_H

// Local includes
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(IsNumericTest) {

        // arrange
        const std::string value1 = "123456";
        const std::string value2 = "abcdef";

        // act
        const bool result1 = StringUtils::IsNumeric(value1);
        const bool result2 = StringUtils::IsNumeric(value2);

        // assert
        BOOST_CHECK_EQUAL(result1, true);
        BOOST_CHECK_EQUAL(result2, false);
    }

    BOOST_AUTO_TEST_CASE(SplitTest) {

        // arrange
        const std::string value1 = "123 456 789";
        const std::string value2 = "123\n456\n789";

        // act
        const std::vector<std::string> result1 = StringUtils::Split(value1, " ");
        const std::vector<std::string> result2 = StringUtils::Split(value2, "\n");

        // assert
        BOOST_CHECK_EQUAL(result1.size(), 3);
        BOOST_CHECK_EQUAL(result2.size(), 3);
    }

    BOOST_AUTO_TEST_CASE(JoinTest) {

        // arrange
        const std::vector<std::string> values = {"transfer-manager", "ftpuser1", "incoming", "mix"};

        // act
        const std::string result = StringUtils::Join(values, "/", 1);

        // assert
        BOOST_CHECK_EQUAL(result, "ftpuser1/incoming/mix");
    }

    BOOST_AUTO_TEST_CASE(IsUUidTest) {

        // arrange
        const std::string value1 = "88c3b756-ebc3-46fd-998e-7a2bfb02f347";
        const std::string value2 = "88C3B756-EBC3-46FD-998E-7A2BFB02F347";
        const std::string value3 = "abscdef";

        // act
        const bool result1 = StringUtils::IsUuid(value1);
        const bool result2 = StringUtils::IsUuid(value2);
        const bool result3 = StringUtils::IsUuid(value3);

        // assert
        BOOST_CHECK_EQUAL(result1, true);
        BOOST_CHECK_EQUAL(result2, true);
        BOOST_CHECK_EQUAL(result3, false);
    }

    BOOST_AUTO_TEST_CASE(StripWhiteSpacesTest) {

        // arrange
        std::string value = "   \r\n\r\n\n   ";

        // act
        const std::string result = StringUtils::StripWhiteSpaces(value);

        // assert
        BOOST_CHECK_EQUAL(result.length(), 0);
    }

    BOOST_AUTO_TEST_CASE(StripNonWhiteSpacesTest) {
        // arrange
        std::string value = "   \r\n\r\n\n   aksjdh";

        // act
        const std::string result = StringUtils::StripWhiteSpaces(value);

        // assert
        BOOST_CHECK_EQUAL(result.length(), 6);
        BOOST_CHECK_EQUAL(result, value);
    }

    BOOST_AUTO_TEST_CASE(EqualTest) {

        // arrange
        const std::string s1 = "The quick brown fox jumps over the lazy dog";
        const std::string s2 = "The quick brown fox jumps over the lazy dog";
        const std::string s3 = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";

        // act
        const bool result1 = StringUtils::Equals(s1, s2);
        const bool result2 = StringUtils::EqualsIgnoreCase(s1, s3);

        // assert
        BOOST_CHECK_EQUAL(result1, true);
        BOOST_CHECK_EQUAL(result2, true);
    }

    BOOST_AUTO_TEST_CASE(ContainsTest) {

        // arrange
        const std::string s1 = "The quick brown fox jumps over the lazy dog";
        const std::string s2 = "quick brown fox";

        // act
        const bool result = StringUtils::Contains(s1, s2);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(StripTest) {

        // arrange
        const std::string s1 = "Threads:        80";
        const std::string s2 = "   80    ";

        // act
        const std::string result1 = StringUtils::Split(s1, ":")[1];
        const std::string result2 = StringUtils::Trim(StringUtils::Split(s1, ":")[1]);
        const std::string result3 = StringUtils::Trim(s2);

        // assert
        BOOST_CHECK_EQUAL(result1, "        80");
        BOOST_CHECK_EQUAL(result2, "80");
        BOOST_CHECK_EQUAL(result3, "80");
    }

    BOOST_AUTO_TEST_CASE(StringDoubleTest) {

        // arrange
        const std::string s1 = " 80 ";

        // act
        const double result1 = std::stod(StringUtils::Trim(s1));

        // assert
        BOOST_CHECK_EQUAL(80.0, result1);
    }

    BOOST_AUTO_TEST_CASE(UrlEncodeTest) {

        // arrange
        const std::string input = "/abc/xyz/\n ";

        // act
        const std::string result = StringUtils::UrlEncode(input);

        // assert
        BOOST_CHECK_EQUAL("/abc/xyz/%0A%20", result.c_str());
    }

    BOOST_AUTO_TEST_CASE(UrlEncodeSpecialCharactersTest) {

        // arrange
        const std::string input = "/abc/xyz/\n +";

        // act
        const std::string result = StringUtils::UrlEncode(input);

        // assert
        BOOST_CHECK_EQUAL("/abc/xyz/%0A%20%2B", result.c_str());
    }

    BOOST_AUTO_TEST_CASE(StripLineEndingsTest) {

        // arrange
        const std::string input = "/abc/xyz/\r\n";

        // act
        const std::string result = StringUtils::StripLineEndings(input);

        // assert
        BOOST_CHECK_EQUAL("/abc/xyz/", result.c_str());
    }

    BOOST_AUTO_TEST_CASE(RemoveColorCodingTest) {

        // arrange
        const std::string input = "2025-05-29 15:38:33.044 [35m[main            ][0;39m [34mINFO [0;39m [36meInitializer.CustomRuntimeInitializer.java:55[0;39m : AWS Handler: org.springframework.cloud.function.adapter.aws.FunctionInvoker";

        // act
        const std::string result = StringUtils::RemoveColorCoding(input);

        // assert
        BOOST_CHECK_EQUAL("2025-05-29 15:38:33.044 [main            ] INFO  eInitializer.CustomRuntimeInitializer.java:55 : AWS Handler: org.springframework.cloud.function.adapter.aws.FunctionInvoker", result.c_str());
    }

    BOOST_AUTO_TEST_CASE(GetBodyTestTest) {

        /*        // arrange
        std::string output =
                "HTTP/1.1 200 OK\r\nApi-Version: 1.42\r\nContent-Type: application/json\r\nDate: Wed, 07 Jun 2023 18:33:56 GMT\r\nDocker-Experimental: false\r\nOstype: linux\r\nServer: Docker/23.0.5 (linux)\r\nConnection: close\r\nTransfer-Encoding: chunked\r\n\r\n2fc\r\n[{\"Id\":\"e1d2c2c69edcc8967ff69f44913fd7160cc2046ab751cc65fe00b83ead81d2e8\",\"Names\":[\"/ftp-file-copy\"],\"Image\":\"ftp-file-copy:latest\",\"ImageID\":\"sha256:fe7db6fa4195ba42f40608245a841abdae1fc3dbb1c51ffc017f195f4000bdea\",\"Command\":\"/lambda-entrypoint.sh org.springframework.cloud.function.adapter.aws.FunctionInvoker::handleRequest\",\"Created\":1686158110,\"Ports\":[],\"Labels\":{\"desktop.docker.io/wsl-distro\":\"Debian\"},\"State\":\"created\",\"Status\":\"Created\",\"HostConfig\":{\"NetworkMode\":\"default\"},\"NetworkSettings\":{\"Networks\":{\"bridge\":{\"IPAMConfig\":null,\"Links\":null,\"Aliases\":null,\"NetworkID\":\"\",\"EndpointID\":\"\",\"Gateway\":\"\",\"IPAddress\":\"\",\"IPPrefixLen\":0,\"IPv6Gateway\":\"\",\"GlobalIPv6Address\":\"\",\"GlobalIPv6PrefixLen\":0,\"MacAddress\":\"\",\"DriverOpts\":null}}},\"Mounts\":[]}]\n\r\n0\r\n\r\n[{\"Id\":\"e1d2c2c69edcc8967ff69f44913fd7160cc2046ab751cc65fe00b83ead81d2e8\",\"Names\":[\"/ftp-file-copy\"],\"Image\":\"ftp-file-copy:latest\",\"ImageID\":\"sha256:fe7db6fa4195ba42f40608245a841abdae1fc3dbb1c51ffc017f195f4000bdea\",\"Command\":\"/lambda-entrypoint.sh org.springframework.cloud.function.adapter.aws.FunctionInvoker::handleRequest\",\"Created\":1686158110,\"Ports\":[],\"Labels\":{\"desktop.docker.io/wsl-distro\":\"Debian\"},\"State\":\"created\",\"Status\":\"Created\",\"HostConfig\":{\"NetworkMode\":\"default\"},\"NetworkSettings\":{\"Networks\":{\"bridge\":{\"IPAMConfig\":null,\"Links\":null,\"Aliases\":null,\"NetworkID\":\"\",\"EndpointID\":\"\",\"Gateway\":\"\",\"IPAddress\":\"\",\"IPPrefixLen\":0,\"IPv6Gateway\":\"\",\"GlobalIPv6Address\":\"\",\"GlobalIPv6PrefixLen\":0,\"MacAddress\":\"\",\"DriverOpts\":null}}},\"Mounts\":[]}]\n\r\n";

        // act
        Poco::RegularExpression regex(R"(.*[\r?|\n?]{2}([0-9a-f]+)[\r?|\n?]{1}(.*)[\r?|\n?]{2}.*)");
        Poco::RegularExpression::MatchVec mvec;
        int matches = regex.match(output, 0, mvec);

        // assert
        EXPECT_EQ(matches, 3);*/
    }

    /*BOOST_AUTO_TEST_CASE(SanitizeUtf8Test) {

        // arrange
        std::string input = "some invalid\xFE\xFE\xFF\xFF\xBB stuff";

        // act
        std::string result = StringUtils::SanitizeUtf8(input);

        // assert
        EXPECT_TRUE(result == "some invalid stuff");
    }*/

    BOOST_AUTO_TEST_CASE(SnakeCaseTest) {

        // arrange
        const std::string input = "CreateQueue";

        // act
        const std::string result = StringUtils::ToSnakeCase(input);

        // assert
        BOOST_CHECK_EQUAL(result, "create-queue");
    }

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_STRING_UTILS_TEST_H
