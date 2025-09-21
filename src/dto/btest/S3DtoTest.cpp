//
// Created by vogje01 on 4/22/25.
//

#ifndef AWMOCK_DTO_S3_TEST_H
#define AWMOCK_DTO_S3_TEST_H

// AwsMock includes
#include "TestBase.h"
#include <awsmock/dto/sqs/ReceiveMessageRequest.h>
#include <awsmock/dto/sqs/ReceiveMessageResponse.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

// Test includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/dto/s3/internal/GetObjectCounterRequest.h>
#include <awsmock/dto/s3/internal/GetObjectCounterResponse.h>
#include <awsmock/dto/s3/internal/ListBucketCounterRequest.h>
#include <awsmock/dto/s3/internal/ListBucketCounterResponse.h>
#include <awsmock/dto/s3/internal/ListObjectCounterRequest.h>
#include <awsmock/dto/s3/internal/ListObjectCounterResponse.h>
#include <awsmock/dto/s3/internal/UpdateLambdaConfigurationRequest.h>
#include <awsmock/dto/s3/internal/UpdateObjectRequest.h>

#define EXPECT_WITHIN_A_SECOND(x, y) EXPECT_TRUE(x.time_since_epoch().count() < y.time_since_epoch().count() && x.time_since_epoch().count() < y.time_since_epoch().count() + 100000)

namespace AwsMock::Dto::S3 {

    class S3DtoTest : TestBase {
    };

    BOOST_FIXTURE_TEST_CASE(GetObjectCounterRequestTest, S3DtoTest) {

        // arrange
        GetObjectCounterRequest request;
        request.region = "region";
        request.oid = "oid";

        // act
        const std::string jsonResult = request.ToJson();
        const GetObjectCounterRequest result = GetObjectCounterRequest::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.oid, "oid");
    }

    BOOST_FIXTURE_TEST_CASE(GetObjectCounterResponseTest, S3DtoTest) {

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
        const GetObjectCounterResponse result = GetObjectCounterResponse::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.user, "user");
        BOOST_CHECK_EQUAL(result.objectCounter.bucketName, "bucketName");
        BOOST_CHECK_CLOSE(static_cast<double>(result.objectCounter.created.time_since_epoch().count()), static_cast<double>(objectCounter.created.time_since_epoch().count()), 3);
        BOOST_CHECK_CLOSE(static_cast<double>(result.objectCounter.modified.time_since_epoch().count()), static_cast<double>(objectCounter.modified.time_since_epoch().count()), 3);
    }

    BOOST_FIXTURE_TEST_CASE(ListBucketCounterRequestTest, S3DtoTest) {

        // arrange
        ListBucketCounterRequest request;
        request.region = "region";
        request.prefix = "prefix";
        request.pageSize = 10;
        request.pageIndex = 0;

        // Sorting
        Common::SortColumn sortColumn;
        sortColumn.sortDirection = 1;
        sortColumn.column = "key";
        request.sortColumns.emplace_back(sortColumn);

        // act
        std::string jsonResult = request.ToJson();
        ListBucketCounterRequest result = ListBucketCounterRequest::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.prefix, "prefix");
        BOOST_CHECK_EQUAL(result.pageSize, 10);
        BOOST_CHECK_EQUAL(result.pageIndex, 0);
        BOOST_CHECK_EQUAL(result.sortColumns.at(0).sortDirection, 1);
        BOOST_CHECK_EQUAL(result.sortColumns.at(0).column, "key");
    }

    BOOST_FIXTURE_TEST_CASE(ListBucketCounterResponseTest, S3DtoTest) {

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
        ListBucketCounterResponse result = ListBucketCounterResponse::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.total, 10);
        BOOST_CHECK_EQUAL(result.bucketCounters.size(), 1);
        BOOST_CHECK_EQUAL(result.bucketCounters[0].bucketName, "bucketName");
        BOOST_CHECK_EQUAL(result.bucketCounters[0].keys, 10);
        BOOST_CHECK_EQUAL(result.bucketCounters[0].size, 10);
        BOOST_CHECK_EQUAL(result.bucketCounters[0].owner, "owner");
        BOOST_CHECK_CLOSE(static_cast<double>(result.bucketCounters[0].created.time_since_epoch().count()), static_cast<double>(result.bucketCounters[0].created.time_since_epoch().count()), 3);
    }

    BOOST_FIXTURE_TEST_CASE(ListObjectCounterRequestTest, S3DtoTest) {

        // arrange
        ListObjectCounterRequest request;
        request.region = "region";
        request.prefix = "prefix";
        request.pageSize = 10;
        request.pageIndex = 0;

        // Sorting
        Common::SortColumn sortColumn;
        sortColumn.sortDirection = 1;
        sortColumn.column = "key";
        request.sortColumns.emplace_back(sortColumn);

        // act
        std::string jsonResult = request.ToJson();
        ListObjectCounterRequest result = ListObjectCounterRequest::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.prefix, "prefix");
        BOOST_CHECK_EQUAL(result.pageSize, 10);
        BOOST_CHECK_EQUAL(result.pageIndex, 0);
        BOOST_CHECK_EQUAL(result.sortColumns.at(0).sortDirection, 1);
        BOOST_CHECK_EQUAL(result.sortColumns.at(0).column, "key");
    }

    BOOST_FIXTURE_TEST_CASE(ListObjectCounterResponseTest, S3DtoTest) {

        // arrange
        ListObjectCounterResponse response;
        response.region = "region";
        response.total = 10;

        ObjectCounter objectCounter;
        objectCounter.bucketName = "bucketName";
        objectCounter.key = "key";
        objectCounter.size = 10;
        objectCounter.oid = "oid";
        objectCounter.created = system_clock::now();
        response.objectCounters.emplace_back(objectCounter);

        // act
        const std::string jsonResult = response.ToJson();
        const ListObjectCounterResponse result = ListObjectCounterResponse::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.total, 10);
        BOOST_CHECK_EQUAL(result.objectCounters.size(), 1);
        BOOST_CHECK_EQUAL(result.objectCounters[0].bucketName, "bucketName");
        BOOST_CHECK_EQUAL(result.objectCounters[0].key, "key");
        BOOST_CHECK_EQUAL(result.objectCounters[0].oid, "oid");
        BOOST_CHECK_EQUAL(result.objectCounters[0].size, 10);
        BOOST_CHECK_CLOSE(static_cast<double>(result.objectCounters[0].created.time_since_epoch().count()), static_cast<double>(result.objectCounters[0].created.time_since_epoch().count()), 3);
    }

    BOOST_FIXTURE_TEST_CASE(UpdateObjectRequestTest, S3DtoTest) {

        // arrange
        UpdateObjectRequest request;
        request.bucket = "bucket";
        request.key = "key";
        request.metadata["key1"] = "value1";

        // act
        std::string jsonResult = request.ToJson();
        UpdateObjectRequest result = UpdateObjectRequest::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.bucket, "bucket");
        BOOST_CHECK_EQUAL(result.key, "key");
        BOOST_CHECK_EQUAL(result.metadata["key1"], "value1");
    }

    BOOST_FIXTURE_TEST_CASE(UpdateLambdaConfigurationRequestTest, S3DtoTest) {

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
        UpdateLambdaConfigurationRequest result = UpdateLambdaConfigurationRequest::FromJson(jsonResult);

        // assert
        BOOST_CHECK_EQUAL(result.region, "region");
        BOOST_CHECK_EQUAL(result.bucket, "bucket");
        BOOST_CHECK_EQUAL(result.lambdaConfiguration.lambdaArn, "arn");
        BOOST_CHECK_EQUAL(result.lambdaConfiguration.id, "id");
        BOOST_CHECK_EQUAL(result.lambdaConfiguration.filterRules.size(), 1);
        BOOST_CHECK_EQUAL(result.lambdaConfiguration.filterRules[0].filterValue, "value");
        BOOST_CHECK_EQUAL(NameTypeToString(result.lambdaConfiguration.filterRules[0].name), NameTypeToString(NameType::prefix));
        BOOST_CHECK_EQUAL(result.lambdaConfiguration.events.size(), 1);
        BOOST_CHECK_EQUAL(EventTypeToString(result.lambdaConfiguration.events[0]), EventTypeToString(NotificationEventType::OBJECT_CREATED));
    }

}// namespace AwsMock::Dto::S3

#endif// AWMOCK_DTO_S3_TEST_H