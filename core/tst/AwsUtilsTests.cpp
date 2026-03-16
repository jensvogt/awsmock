//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(CreateS3ArnTest) {

        // arrange
        const std::string bucket = "testBucket";
        const std::string key = "testKey";
        Configuration::instance().SetFilename(TMP_CONFIGURATION_FILE);
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const auto s3Arn = "arn:aws:s3:" + region + ":" + accountId + ":" + bucket + "/" + key;

        // act
        const std::string result = AwsUtils::CreateS3ObjectArn(region, accountId, bucket, key);

        // assert
        BOOST_CHECK_EQUAL(result, s3Arn);
    }

    BOOST_AUTO_TEST_CASE(CreateSqsArnTest) {

        // arrange
        const std::string queueName = "testQueue";
        Configuration::instance().SetFilename(TMP_CONFIGURATION_FILE);
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const std::string sqsQueueArn = "arn:aws:sqs:" + region + ":" + accountId + ":" + queueName;

        // act
        const std::string result = AwsUtils::CreateSQSQueueArn(region, accountId, queueName);

        // assert
        BOOST_CHECK_EQUAL(result, sqsQueueArn);
    }

    BOOST_AUTO_TEST_CASE(CreateSNSTopicTest) {

        // arrange
        const std::string topicName = "testTopic";
        Configuration::instance().SetFilename(TMP_CONFIGURATION_FILE);
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const auto snsTopicArn = "arn:aws:sns:" + region + ":" + accountId + ":" + topicName;

        // act
        const std::string result = AwsUtils::CreateSNSTopicArn(region, accountId, topicName);

        // assert
        BOOST_CHECK_EQUAL(result.c_str(), snsTopicArn.c_str());
    }

    BOOST_AUTO_TEST_CASE(CreateLambdaTopicTest) {

        // arrange
        const std::string topicName = "testTopic";
        Configuration::instance().SetFilename(TMP_CONFIGURATION_FILE);
        const std::string functionName = "test-function";
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const auto lambdaArn = "arn:aws:lambda:" + region + ":" + accountId + ":function:" + functionName;

        // act
        const std::string result = AwsUtils::CreateLambdaArn(region, accountId, functionName);

        // assert
        BOOST_CHECK_EQUAL(result.c_str(), lambdaArn.c_str());
    }

    BOOST_AUTO_TEST_CASE(ConvertArnToUrlTest) {

        // arrange
        const std::string queueName = "file-delivery1-queue";
        Configuration::instance().SetFilename(TMP_CONFIGURATION_FILE);
        const auto port = Configuration::instance().GetValue<std::string>("awsmock.gateway.http.port");
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const std::string endpoint = SystemUtils::GetHostName() + ":" + port;
        const std::string sqsQueueUrl = "http://sqs." + region + "." + endpoint + "/" + accountId + "/" + queueName;
        const std::string sqsQueueArn = "arn:aws:sqs:" + region + ":" + accountId + ":" + queueName;

        // act
        const std::string result = AwsUtils::ConvertSQSQueueArnToUrl(sqsQueueArn);

        // assert
        BOOST_CHECK_EQUAL(result.c_str(), sqsQueueUrl.c_str());
    }

    BOOST_AUTO_TEST_CASE(ConvertUrlToArnTest) {

        // arrange
        const std::string queueName = "file-delivery1-queue";
        Configuration::instance().SetFilename(TMP_CONFIGURATION_FILE);
        const auto port = Configuration::instance().GetValue<std::string>("awsmock.gateway.http.port");
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const std::string endpoint = SystemUtils::GetHostName() + ":" + port;
        const std::string sqsQueueUrl = "http://sqs." + region + "." + endpoint + "/" + accountId + "/" + queueName;
        const std::string sqsQueueArn = "arn:aws:sqs:" + region + ":" + accountId + ":" + queueName;

        // act
        const std::string result = AwsUtils::ConvertToArn(region, sqsQueueUrl);

        // assert
        BOOST_CHECK_EQUAL(result, sqsQueueArn);
    }

    BOOST_AUTO_TEST_CASE(IsURLTest) {

        // arrange
        const auto port = Configuration::instance().GetValue<std::string>("awsmock.gateway.http.port");
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const std::string endpoint = SystemUtils::GetHostName() + ":" + port;
        const std::string queueName1 = "http://sqs." + region + "." + endpoint + "/" + accountId + "/test";
        const std::string queueName2 = "ftp://sqs." + region + "." + endpoint + "/" + accountId + "/test";

        // act
        const bool result1 = AwsUtils::IsUrl(queueName1);
        const bool result2 = AwsUtils::IsUrl(queueName2);

        // assert
        BOOST_CHECK_EQUAL(true, result1);
        BOOST_CHECK_EQUAL(false, result2);
    }

    BOOST_AUTO_TEST_CASE(IsArnTest) {

        // arrange
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const std::string arn1 = "arn:aws:lambda:" + region + ":" + accountId + ":test";
        const std::string arn2 = "arn:aws:lambda:" + region + ":" + accountId + ":test/test1";
        const std::string arn3 = "arn:abc:lambda:" + region + ":" + accountId + ":test/test1";

        // act
        const bool result1 = AwsUtils::IsArn(arn1);
        const bool result2 = AwsUtils::IsArn(arn2);
        const bool result3 = AwsUtils::IsArn(arn3);

        // assert
        BOOST_CHECK_EQUAL(true, result1);
        BOOST_CHECK_EQUAL(true, result2);
        BOOST_CHECK_EQUAL(false, result3);
    }

    /**
     * @brief Test the AWS4 signature
     *
     * From: @link https://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html @endlink
     *
     * GET /test.txt HTTP/1.1
     * Host: examplebucket.s3.amazonaws.com
     * Authorization: SignatureToBeCalculated
     * Range: bytes=0-9
     * x-amz-content-sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
     * x-amz-date: 20130524T000000Z
     */
    BOOST_AUTO_TEST_CASE(VerifySignatureTest, *boost::unit_test::disabled()) {

        // arrange
        http::request<http::dynamic_body> request;
        request.method(http::verb::get);
        request.target("/test.txt");
        request.set(http::field::host, "examplebucket.s3.amazonaws.com");
        request.set(http::field::authorization, "AWS4-HMAC-SHA256 Credential=AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request,SignedHeaders=host;range;x-amz-content-sha256;x-amz-date,Signature=1a276044bd879aeb04a95effe35bcddeee46de6d2689e6ff12ec4d23d8e06274");
        request.set(http::field::range, "bytes=0-9");
        request.set("x-amz-content-sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        request.set("x-amz-date", "20130524T000000Z");
        const std::string secretAccessKey = "wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY";

        // act
        const bool result = AwsUtils::VerifySignature(request, secretAccessKey);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }
}// namespace AwsMock::Core