//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_CORE_STRING_UTILS_TEST_H
#define AWSMOCK_CORE_STRING_UTILS_TEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Core {

    class StringUtilsTest : public ::testing::Test {};

    TEST_F(StringUtilsTest, IsNumericTest) {

        // arrange
        const std::string value1 = "123456";
        const std::string value2 = "abcdef";

        // act
        const bool result1 = StringUtils::IsNumeric(value1);
        const bool result2 = StringUtils::IsNumeric(value2);

        // assert
        EXPECT_TRUE(result1);
        EXPECT_FALSE(result2);
    }

    TEST_F(StringUtilsTest, SplitTest) {

        // arrange
        const std::string value1 = "123 456 789";
        const std::string value2 = "123\n456\n789";

        // act
        const std::vector<std::string> result1 = StringUtils::Split(value1, ' ');
        const std::vector<std::string> result2 = StringUtils::Split(value2, '\n');

        // assert
        EXPECT_EQ(result1.size(), 3);
        EXPECT_EQ(result2.size(), 3);
    }

    TEST_F(StringUtilsTest, JoinTest) {

        // arrange
        const std::vector<std::string> values = {"transfer-manager", "ftpuser1", "incoming", "mix"};

        // act
        const std::string result = StringUtils::Join(values, "/", 1);

        // assert
        EXPECT_TRUE(result == "ftpuser1/incoming/mix");
    }

    TEST_F(StringUtilsTest, IsUUidTest) {

        // arrange
        const std::string value1 = "88c3b756-ebc3-46fd-998e-7a2bfb02f347";
        const std::string value2 = "88C3B756-EBC3-46FD-998E-7A2BFB02F347";
        const std::string value3 = "abscdef";

        // act
        const bool result1 = StringUtils::IsUuid(value1);
        const bool result2 = StringUtils::IsUuid(value2);
        const bool result3 = StringUtils::IsUuid(value3);

        // assert
        EXPECT_TRUE(result1);
        EXPECT_TRUE(result2);
        EXPECT_FALSE(result3);
    }

    TEST_F(StringUtilsTest, StripWhiteSpacesTest) {

        // arrange
        std::string value = "   \r\n\r\n\n   ";

        // act
        const std::string result = StringUtils::StripWhiteSpaces(value);

        // assert
        EXPECT_EQ(result.length(), 0);
    }

    TEST_F(StringUtilsTest, StripNonWhiteSpacesTest) {
        // arrange
        std::string value = "   \r\n\r\n\n   aksjdh";

        // act
        const std::string result = StringUtils::StripWhiteSpaces(value);

        // assert
        EXPECT_EQ(result.length(), 6);
        EXPECT_STREQ(result.c_str(), value.c_str());
    }

    TEST_F(StringUtilsTest, EqualTest) {

        // arrange
        const std::string s1 = "The quick brown fox jumps over the lazy dog";
        const std::string s2 = "The quick brown fox jumps over the lazy dog";
        const std::string s3 = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";

        // act
        const bool result1 = StringUtils::Equals(s1, s2);
        const bool result2 = StringUtils::EqualsIgnoreCase(s1, s3);

        // assert
        EXPECT_TRUE(result1);
        EXPECT_TRUE(result2);
    }

    TEST_F(StringUtilsTest, ContainsTest) {

        // arrange
        const std::string s1 = "The quick brown fox jumps over the lazy dog";
        const std::string s2 = "quick brown fox";

        // act
        const bool result = StringUtils::Contains(s1, s2);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(StringUtilsTest, StripTest) {

        // arrange
        const std::string s1 = "Threads:        80";
        const std::string s2 = "   80    ";

        // act
        const std::string result1 = StringUtils::Split(s1, ':')[1];
        const std::string result2 = StringUtils::Trim(StringUtils::Split(s1, ':')[1]);
        const std::string result3 = StringUtils::Trim(s2);

        // assert
        EXPECT_STREQ(result1.c_str(), "        80");
        EXPECT_STREQ(result2.c_str(), "80");
        EXPECT_STREQ(result3.c_str(), "80");
    }

    TEST_F(StringUtilsTest, StringDoubleTest) {

        // arrange
        const std::string s1 = " 80 ";

        // act
        const double result1 = std::stod(StringUtils::Trim(s1));

        // assert
        EXPECT_EQ(80.0, result1);
    }

    TEST_F(StringUtilsTest, UrlEncodeTest) {

        // arrange
        const std::string input = "/abc/xyz/\n ";

        // act
        const std::string result = StringUtils::UrlEncode(input);

        // assert
        EXPECT_STREQ("/abc/xyz/%0A%20", result.c_str());
    }

    TEST_F(StringUtilsTest, UrlEncodeSpecialCharactersTest) {

        // arrange
        const std::string input = "/abc/xyz/\n +";

        // act
        const std::string result = StringUtils::UrlEncode(input);

        // assert
        EXPECT_STREQ("/abc/xyz/%0A%20%2B", result.c_str());
    }

    TEST_F(StringUtilsTest, StripLineEndingsTest) {

        // arrange
        const std::string input = "/abc/xyz/\r\n";

        // act
        const std::string result = StringUtils::StripLineEndings(input);

        // assert
        EXPECT_STREQ("/abc/xyz/", result.c_str());
    }

    TEST_F(StringUtilsTest, GetBodyTestTest) {

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

    /*TEST_F(StringUtilsTest, SanitizeUtf8Test) {

        // arrange
        std::string input = "some invalid\xFE\xFE\xFF\xFF\xBB stuff";

        // act
        std::string result = StringUtils::SanitizeUtf8(input);

        // assert
        EXPECT_TRUE(result == "some invalid stuff");
    }*/

    TEST_F(StringUtilsTest, SnakeCaseTest) {

        // arrange
        const std::string input = "CreateQueue";

        // act
        const std::string result = StringUtils::ToSnakeCase(input);

        // assert
        EXPECT_TRUE(result == "create-queue");
    }

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_STRING_UTILS_TEST_H
