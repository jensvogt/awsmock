//
// Created by vogje01 on 21/10/2023.
//

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define BOOST_TEST_MODULE S3ServiceJavaTests
#define REGION std::string("eu-central-1")
#define TEST_BUCKET std::string("test-bucket")
#define TEST_KEY std::string("test-key")
#define TEST_BUCKET_COPY std::string("test-bucket-copy")
#define TEST_KEY_COPY std::string("test-key-copy")
#define TEST_PORT 10100

namespace AwsMock::Service {

    /**
     * @brief Tests the aws-sdk-java interface to the AwsMock system.
     *
     * @par
     * The awsmock-test docker image will be started during startup.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct S3ServiceJavaTests : TestBase {

        S3ServiceJavaTests() {

            // Start the gateway server
            StartGateway(TEST_PORT);

            // General configuration
            _region = GetRegion();

            // Base URL
            _baseUrl = "/api/s3/";

            // Cleanup
            long deleted = _s3Database.DeleteAllObjects();
            log_debug << "Deleted objects: " << deleted << " objects";
            deleted = _s3Database.DeleteAllBuckets();
            log_debug << "Deleted buckets: " << deleted << " objects";
        }

        static Core::HttpSocketResponse SendGetCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::get, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendPostCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendPutCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::put, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendDeleteCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::delete_, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        std::string _region, _baseUrl;
        Database::S3Database &_s3Database = Database::S3Database::instance();
    };

    BOOST_FIXTURE_TEST_CASE(S3CreateBucketTest, S3ServiceJavaTests) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        const long buckets = _s3Database.BucketCount();

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, buckets);
    }

    BOOST_FIXTURE_TEST_CASE(S3ListBucketTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);

        // act
        const Core::HttpSocketResponse listResult = SendGetCommand(_baseUrl + "listBuckets", {});

        // assert
        BOOST_CHECK_EQUAL(listResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, buckets);
    }
    /*
    BOOST_FIXTURE_TEST_CASE(S3PutBucketVersioningTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);

        // act
        const Core::HttpSocketResponse versioningResult = SendPutCommand(_baseUrl + "putBucketVersioning?bucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        const Database::Entity::S3::Bucket bucket = _s3Database.GetBucketByRegionName(REGION, TEST_BUCKET);

        // assert
        BOOST_CHECK_EQUAL(versioningResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(bucket.versionStatus == Database::Entity::S3::BucketVersionStatus::ENABLED, true);
    }

    BOOST_FIXTURE_TEST_CASE(S3ListObjectVersionsTest, S3ServiceJavaTests) {

        // arrange
        Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);

        Core::HttpSocketResponse versioningResult = SendPutCommand(_baseUrl + "putBucketVersioning?bucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        Database::Entity::S3::Bucket bucket = _s3Database.GetBucketByRegionName(REGION, TEST_BUCKET);
        BOOST_CHECK_EQUAL(versioningResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(bucket.versionStatus == Database::Entity::S3::BucketVersionStatus::ENABLED, true);

        Core::HttpSocketResponse putObjectResult1 = SendPutCommand(_baseUrl + "putObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        BOOST_CHECK_EQUAL(putObjectResult1.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, _s3Database.ObjectCount(REGION, {}, TEST_BUCKET));

        Core::HttpSocketResponse putObjectResult2 = SendPutCommand(_baseUrl + "putObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        BOOST_CHECK_EQUAL(putObjectResult2.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(2, _s3Database.ObjectCount(REGION, {}, TEST_BUCKET));

        // act
        Core::HttpSocketResponse listVersionsResult = SendGetCommand(_baseUrl + "listObjectVersions?bucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&prefix=test", {});

        // assert
        BOOST_CHECK_EQUAL(listVersionsResult.statusCode == http::status::ok, true);
        int versions = std::stoi(listVersionsResult.body);
        BOOST_CHECK_EQUAL(2, versions);
    }
*/
    BOOST_FIXTURE_TEST_CASE(S3DeleteBucketTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);

        // act
        const Core::HttpSocketResponse deleteResult = SendDeleteCommand(_baseUrl + "deleteBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        buckets = _s3Database.BucketCount();

        // assert
        BOOST_CHECK_EQUAL(deleteResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, buckets);
    }

    BOOST_FIXTURE_TEST_CASE(S3PutObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);

        // act
        const Core::HttpSocketResponse putObjectResult = SendPutCommand(_baseUrl + "putObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        const long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);

        // assert
        BOOST_CHECK_EQUAL(putObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, objects);
    }

    BOOST_FIXTURE_TEST_CASE(S3GetObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        Core::HttpSocketResponse putObjectResult = SendPutCommand(_baseUrl + "putObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // act
        const Core::HttpSocketResponse getObjectResult = SendGetCommand(_baseUrl + "getObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY), {});
        objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);

        // assert
        BOOST_CHECK_EQUAL(getObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, objects);
    }

    BOOST_FIXTURE_TEST_CASE(S3GetSizeTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        Core::HttpSocketResponse putObjectResult = SendPutCommand(_baseUrl + "putObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        const long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // act
        const Core::HttpSocketResponse getObjectResult = SendGetCommand(_baseUrl + "getHead?bucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY), {});
        const long size = std::stol(getObjectResult.body);

        // assert
        BOOST_CHECK_EQUAL(getObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(size > 0, true);
    }

    BOOST_FIXTURE_TEST_CASE(S3UploadObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);

        // act
        const Core::HttpSocketResponse uploadObjectResult = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=10", {});
        const long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // assert
        BOOST_CHECK_EQUAL(uploadObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, objects);
    }

    BOOST_FIXTURE_TEST_CASE(S3DownloadObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        const Core::HttpSocketResponse uploadObjectResult = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=10", {});
        BOOST_CHECK_EQUAL(uploadObjectResult.statusCode == http::status::ok, true);
        const long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // act
        const Core::HttpSocketResponse downloadObjectResult = SendGetCommand(_baseUrl + "downloadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY), {});

        // assert
        BOOST_CHECK_EQUAL(downloadObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, objects);
    }

    BOOST_FIXTURE_TEST_CASE(S3CopyObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult1 = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult1.statusCode == http::status::ok, true);
        long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        const Core::HttpSocketResponse createResult2 = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET_COPY), {});
        BOOST_CHECK_EQUAL(createResult2.statusCode == http::status::ok, true);
        buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(2, buckets);
        const Core::HttpSocketResponse uploadObjectResult = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        BOOST_CHECK_EQUAL(uploadObjectResult.statusCode == http::status::ok, true);
        long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // act
        const Core::HttpSocketResponse copyObjectResult = SendPostCommand(_baseUrl + "copyObject?sourceBucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&sourceKey=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&destinationBucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET_COPY) + "&destinationKey=" + Core::StringUtils::UrlEncode(TEST_KEY_COPY), {});
        BOOST_CHECK_EQUAL(copyObjectResult.statusCode == http::status::ok, true);
        const long objects1 = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        const long objects2 = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET_COPY);

        // assert
        BOOST_CHECK_EQUAL(copyObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, objects1);
        BOOST_CHECK_EQUAL(1, objects2);
    }

    BOOST_FIXTURE_TEST_CASE(S3CopyBigObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult1 = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult1.statusCode == http::status::ok, true);
        long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        const Core::HttpSocketResponse createResult2 = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET_COPY), {});
        BOOST_CHECK_EQUAL(createResult2.statusCode == http::status::ok, true);
        buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(2, buckets);
        const Core::HttpSocketResponse uploadObjectResult = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=200", {});
        BOOST_CHECK_EQUAL(uploadObjectResult.statusCode == http::status::ok, true);
        const long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // act
        const Core::HttpSocketResponse copyObjectResult = SendPostCommand(_baseUrl + "copyBigObject?sourceBucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&sourceKey=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&destinationBucket=" + Core::StringUtils::UrlEncode(TEST_BUCKET_COPY) + "&destinationKey=" + Core::StringUtils::UrlEncode(TEST_KEY_COPY), {});
        BOOST_CHECK_EQUAL(copyObjectResult.statusCode == http::status::ok, true);
        const long objects1 = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        const long objects2 = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET_COPY);

        // assert
        BOOST_CHECK_EQUAL(1, objects1);
        BOOST_CHECK_EQUAL(1, objects2);
    }

    BOOST_FIXTURE_TEST_CASE(S3DeleteObjectTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        const Core::HttpSocketResponse uploadObjectResult = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        BOOST_CHECK_EQUAL(uploadObjectResult.statusCode == http::status::ok, true);
        long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);

        // act
        const Core::HttpSocketResponse deleteObjectResult = SendDeleteCommand(_baseUrl + "deleteObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY), {});
        BOOST_CHECK_EQUAL(deleteObjectResult.statusCode == http::status::ok, true);
        objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);

        // assert
        BOOST_CHECK_EQUAL(deleteObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, objects);
    }

    BOOST_FIXTURE_TEST_CASE(S3DeleteObjectsTest, S3ServiceJavaTests) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createBucket?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET), {});
        BOOST_CHECK_EQUAL(createResult.statusCode == http::status::ok, true);
        const long buckets = _s3Database.BucketCount();
        BOOST_CHECK_EQUAL(1, buckets);
        const Core::HttpSocketResponse uploadObjectResult1 = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&size=1", {});
        BOOST_CHECK_EQUAL(uploadObjectResult1.statusCode == http::status::ok, true);
        long objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(1, objects);
        const Core::HttpSocketResponse uploadObjectResult2 = SendGetCommand(_baseUrl + "uploadObject?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key=" + Core::StringUtils::UrlEncode(TEST_KEY_COPY) + "&size=1", {});
        BOOST_CHECK_EQUAL(uploadObjectResult2.statusCode == http::status::ok, true);
        objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);
        BOOST_CHECK_EQUAL(2, objects);

        // act
        const Core::HttpSocketResponse deleteObjectResult = SendDeleteCommand(_baseUrl + "deleteObjects?bucketName=" + Core::StringUtils::UrlEncode(TEST_BUCKET) + "&key1=" + Core::StringUtils::UrlEncode(TEST_KEY) + "&key2=" + Core::StringUtils::UrlEncode(TEST_KEY_COPY), {});
        BOOST_CHECK_EQUAL(deleteObjectResult.statusCode == http::status::ok, true);
        objects = _s3Database.ObjectCount(REGION, {}, TEST_BUCKET);

        // assert
        BOOST_CHECK_EQUAL(deleteObjectResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, objects);
    }

}// namespace AwsMock::Service
