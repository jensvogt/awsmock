//
// Created by vogje01 on 4/22/25.
//

#ifndef AWMOCK_DTO_S3_LAMBDA_CONFIGURATION_TEST_H
#define AWMOCK_DTO_S3_LAMBDA_CONFIGURATION_TEST_H

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/dto/sqs/ReceiveMessageRequest.h>
#include <awsmock/dto/sqs/ReceiveMessageResponse.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

// Test includes
#include "awsmock/dto/s3/internal/ListBucketCounterResponse.h"


#include <awsmock/core/TestUtils.h>
#include <awsmock/dto/s3/internal/GetObjectCounterRequest.h>
#include <awsmock/dto/s3/internal/GetObjectCounterResponse.h>
#include <awsmock/dto/s3/internal/ListBucketCounterRequest.h>
#include <awsmock/dto/s3/internal/UpdateLambdaConfigurationRequest.h>
#include <awsmock/dto/s3/internal/UpdateObjectRequest.h>

#define EXPECT_WITHIN_A_SECOND(x, y) EXPECT_TRUE(x.time_since_epoch().count() < y.time_since_epoch().count() && x.time_since_epoch().count() < y.time_since_epoch().count() + 100000)

namespace AwsMock::Dto::S3 {

    class S3DtoTest : public ::testing::Test {
      protected:

        void SetUp() override {
        }

        void TearDown() override {
        }
    };

    TEST_F(S3DtoTest, GetObjectCounterRequestTest) {

        // arrange
        GetObjectCounterRequest request;
        request.region = "region";
        request.oid = "oid";

        // act
        const std::string jsonResult = request.ToJson();
        const GetObjectCounterRequest result = request.FromJson(jsonResult);

        // assert
        EXPECT_TRUE(result.region == "region");
        EXPECT_TRUE(result.oid == "oid");
    }

    TEST_F(S3DtoTest, GetObjectCounterResponseTest) {

        // arrange
        GetObjectCounterResponse response;
        response.region = "region";
        response.user = "user";
        response.requestId = "requestId";

        ObjectCounter objectCounter;
        objectCounter.oid = "oid";
        objectCounter.bucketName = "bucketName";
        objectCounter.key = "key";
        objectCounter.contentType = "contentType";
        objectCounter.internalName = "internalName";
        objectCounter.size = 10;
        objectCounter.metadata["key"] = "value";
        objectCounter.created = system_clock::now();
        objectCounter.modified = system_clock::now();
        response.objectCounter = objectCounter;

        // act
        const std::string jsonResult = response.ToJson();
        const GetObjectCounterResponse result = response.FromJson(jsonResult);

        // assert
        EXPECT_TRUE(result.region == "region");
        EXPECT_TRUE(result.user == "user");
        EXPECT_TRUE(result.objectCounter.bucketName == "bucketName");
        EXPECT_WITHIN_A_SECOND(result.objectCounter.created, objectCounter.created);
        EXPECT_WITHIN_A_SECOND(result.objectCounter.modified, objectCounter.modified);
    }

    TEST_F(S3DtoTest, ListBucketCounterRequestTest) {

        // arrange
        ListBucketCounterRequest request;
        request.region = "region";
        request.prefix = "prefix";
        request.pageSize = 10;
        request.pageIndex = 0;

        // act
        std::string jsonResult = request.ToJson();
        ListBucketCounterRequest result = request.FromJson(jsonResult);

        // assert
        EXPECT_TRUE(result.region == "region");
        EXPECT_TRUE(result.prefix == "prefix");
        EXPECT_TRUE(result.pageSize == 10);
        EXPECT_TRUE(result.pageIndex == 0);
    }

    TEST_F(S3DtoTest, ListBucketCounterResponseTest) {

        // arrange
        ListBucketCounterResponse response;
        response.region = "region";
        response.total = 10;

        BucketCounter bucketCounter;
        bucketCounter.bucketName = "bucketName";
        bucketCounter.keys = 10;
        bucketCounter.size = 10;
        bucketCounter.owner = "owner";
        bucketCounter.created = system_clock::now();
        response.bucketCounters.emplace_back(bucketCounter);

        // act
        std::string jsonResult = response.ToJson();
        ListBucketCounterResponse result = response.FromJson(jsonResult);

        // assert
        EXPECT_TRUE(result.region == "region");
        EXPECT_TRUE(result.total == 10);
        EXPECT_TRUE(result.bucketCounters.size() == 1);
        EXPECT_TRUE(result.bucketCounters[0].bucketName == "bucketName");
        EXPECT_WITHIN_A_SECOND(result.bucketCounters[0].created, bucketCounter.created);
        EXPECT_TRUE(result.bucketCounters[0].keys == 10);
        EXPECT_TRUE(result.bucketCounters[0].size == 10);
        EXPECT_TRUE(result.bucketCounters[0].owner == "owner");
    }

    TEST_F(S3DtoTest, UpdateObjectRequestTest) {

        // arrange
        UpdateObjectRequest request;
        request.bucket = "bucket";
        request.key = "key";
        request.metadata["key1"] = "value1";

        // act
        std::string jsonResult = request.ToJson();
        UpdateObjectRequest result = request.FromJson(jsonResult);

        // assert
        EXPECT_TRUE(result.bucket == "bucket");
        EXPECT_TRUE(result.key == "key");
        EXPECT_TRUE(result.metadata["key1"] == "value1");
    }

    TEST_F(S3DtoTest, UpdateLambdaConfigurationRequestTest) {

        // arrange
        UpdateLambdaConfigurationRequest request;
        request.region = "region";
        request.bucket = "bucket";

        // Lambda configuration
        LambdaConfiguration lambdaConfiguration;
        lambdaConfiguration.lambdaArn = "arn";
        lambdaConfiguration.id = "id";

        // Filter rules
        FilterRule filterRule;
        filterRule.filterValue = "value";
        filterRule.name = NameType::prefix;
        lambdaConfiguration.filterRules.emplace_back(filterRule);
        lambdaConfiguration.events.emplace_back(NotificationEventType::OBJECT_CREATED);

        request.lambdaConfiguration = lambdaConfiguration;

        // act
        std::string jsonResult = request.ToJson();
        UpdateLambdaConfigurationRequest result = request.FromJson(jsonResult);

        // assert
        EXPECT_TRUE(result.region == "region");
        EXPECT_TRUE(result.bucket == "bucket");
        EXPECT_TRUE(result.lambdaConfiguration.lambdaArn == "arn");
        EXPECT_TRUE(result.lambdaConfiguration.id == "id");
        EXPECT_TRUE(result.lambdaConfiguration.filterRules.size() == 1);
        EXPECT_TRUE(result.lambdaConfiguration.filterRules[0].filterValue == "value");
        EXPECT_TRUE(result.lambdaConfiguration.filterRules[0].name == NameType::prefix);
        EXPECT_TRUE(result.lambdaConfiguration.events.size() == 1);
        EXPECT_TRUE(result.lambdaConfiguration.events[0] == NotificationEventType::OBJECT_CREATED);
    }

}// namespace AwsMock::Dto::S3

#endif// AWMOCK_DTO_S3_LAMBDA_CONFIGURATION_TEST_H