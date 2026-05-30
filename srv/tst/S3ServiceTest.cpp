//
// Created by vogje01 on 30/05/2023.
//

// C++ standard includes
#include <sstream>

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/s3/S3Service.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_OWNER "test-owner"
#define TEST_BUCKET "test-bucket"
#define TEST_KEY "test-key.txt"
#define TEST_COPY_KEY "test-key-copy.txt"
#define TEST_CONTENT "Hello, S3!"

namespace AwsMock::Database {

    Dto::S3::CreateBucketResponse CreateDefaultBucket(const Service::S3Service &s3Service) {
        Dto::S3::CreateBucketRequest request;
        request.region = TEST_REGION;
        request.name = TEST_BUCKET;
        request.owner = TEST_OWNER;
        return s3Service.CreateBucket(request);
    }

    Dto::S3::PutObjectResponse PutDefaultObject(Service::S3Service &s3Service, const std::string &key = TEST_KEY) {
        Dto::S3::PutObjectRequest request;
        request.region = TEST_REGION;
        request.bucket = TEST_BUCKET;
        request.key = key;
        request.owner = TEST_OWNER;
        request.contentType = "text/plain";
        request.contentLength = static_cast<long>(std::string(TEST_CONTENT).size());
        std::istringstream stream(TEST_CONTENT);
        return s3Service.PutObject(request, stream);
    }

    struct S3ServiceFixture {
        S3ServiceFixture() = default;
        ~S3ServiceFixture() {
            try {
                const long deletedObjects = S3Database::instance().DeleteAllObjects();
                log_debug << "S3 objects deleted, count: " << deletedObjects;
                const long deletedBuckets = S3Database::instance().DeleteAllBuckets();
                log_debug << "S3 buckets deleted, count: " << deletedBuckets;
            } catch (const std::exception &exc) {
                log_error << "S3 fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(S3ServiceTests, S3ServiceFixture)

    BOOST_AUTO_TEST_CASE(BucketCreateTest) {

        // arrange
        const Service::S3Service s3Service;
        Dto::S3::CreateBucketRequest request;
        request.region = TEST_REGION;
        request.name = TEST_BUCKET;
        request.owner = TEST_OWNER;

        // act
        const Dto::S3::CreateBucketResponse response = s3Service.CreateBucket(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.arn.empty());
        //BOOST_CHECK_EQUAL(false, response.location.empty());
    }

    BOOST_AUTO_TEST_CASE(BucketExistsTest) {

        // arrange
        const Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);

        // act
        const bool exists = s3Service.BucketExists(TEST_REGION, TEST_BUCKET);

        // assert
        BOOST_CHECK_EQUAL(true, exists);
    }

    BOOST_AUTO_TEST_CASE(ListAllBucketsTest) {

        // arrange
        const Service::S3Service s3Service;
        Dto::S3::CreateBucketRequest request1;
        request1.region = TEST_REGION;
        request1.name = "test-bucket-1";
        request1.owner = TEST_OWNER;
        Dto::S3::CreateBucketResponse createResponse1 = s3Service.CreateBucket(request1);
        BOOST_CHECK_EQUAL(false, createResponse1.arn.empty());

        Dto::S3::CreateBucketRequest request2;
        request2.region = TEST_REGION;
        request2.name = "test-bucket-2";
        request2.owner = TEST_OWNER;
        Dto::S3::CreateBucketResponse createResponse2 = s3Service.CreateBucket(request2);
        BOOST_CHECK_EQUAL(false, createResponse2.arn.empty());

        // act
        const Dto::S3::ListAllBucketResponse response = s3Service.ListAllBuckets();

        // assert
        BOOST_CHECK_EQUAL(2, response.total);
        BOOST_CHECK_EQUAL(2, response.bucketList.size());
    }

    BOOST_AUTO_TEST_CASE(ListBucketTest) {

        // arrange
        Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);
        PutDefaultObject(s3Service, "key-1.txt");
        PutDefaultObject(s3Service, "key-2.txt");
        s3Service.ReloadAllCounters();

        Dto::S3::ListBucketRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.name = TEST_BUCKET;

        // act
        const Dto::S3::ListBucketResponse response = s3Service.ListBucket(listRequest);

        // assert
        BOOST_CHECK_EQUAL(2, response.keyCount);
        BOOST_CHECK_EQUAL(2, response.contents.size());
    }

    BOOST_AUTO_TEST_CASE(BucketDeleteTest) {

        // arrange
        const Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);
        BOOST_CHECK_EQUAL(true, s3Service.BucketExists(TEST_REGION, TEST_BUCKET));

        Dto::S3::DeleteBucketRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.bucket = TEST_BUCKET;
        deleteRequest.owner = TEST_OWNER;

        // act
        BOOST_CHECK_NO_THROW(s3Service.DeleteBucket(deleteRequest));

        // assert
        BOOST_CHECK_EQUAL(false, s3Service.BucketExists(TEST_REGION, TEST_BUCKET));
    }

    BOOST_AUTO_TEST_CASE(BucketPurgeTest) {

        // arrange
        Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);
        PutDefaultObject(s3Service, "key-1.txt");
        PutDefaultObject(s3Service, "key-2.txt");

        Dto::S3::PurgeBucketRequest purgeRequest;
        purgeRequest.region = TEST_REGION;
        purgeRequest.bucketName = TEST_BUCKET;

        // act
        const long deleted = s3Service.PurgeBucket(purgeRequest);

        // assert
        BOOST_CHECK_EQUAL(2, deleted);

        Dto::S3::ListBucketRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.name = TEST_BUCKET;
        const Dto::S3::ListBucketResponse listResponse = s3Service.ListBucket(listRequest);
        BOOST_CHECK_EQUAL(0, listResponse.keyCount);
    }

    BOOST_AUTO_TEST_CASE(ObjectPutTest) {

        // arrange
        Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);

        Dto::S3::PutObjectRequest request;
        request.region = TEST_REGION;
        request.bucket = TEST_BUCKET;
        request.key = TEST_KEY;
        request.owner = TEST_OWNER;
        request.contentType = "text/plain";
        request.contentLength = std::string(TEST_CONTENT).size();
        std::istringstream stream(TEST_CONTENT);

        // act
        const Dto::S3::PutObjectResponse response = s3Service.PutObject(request, stream);

        // assert
        BOOST_CHECK_EQUAL(TEST_BUCKET, response.bucket);
        BOOST_CHECK_EQUAL(TEST_KEY, response.key);
        BOOST_CHECK_EQUAL(false, response.etag.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectGetTest) {

        // arrange
        Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);
        PutDefaultObject(s3Service);

        Dto::S3::GetObjectRequest request;
        request.region = TEST_REGION;
        request.bucket = TEST_BUCKET;
        request.key = TEST_KEY;

        // act
        const Dto::S3::GetObjectResponse response = s3Service.GetObject(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_BUCKET, response.bucket);
        BOOST_CHECK_EQUAL(TEST_KEY, response.key);
        BOOST_CHECK_EQUAL(false, response.filename.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectDeleteTest) {

        // arrange
        Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);
        PutDefaultObject(s3Service);

        Dto::S3::DeleteObjectRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.bucket = TEST_BUCKET;
        deleteRequest.key = TEST_KEY;

        // act
        BOOST_CHECK_NO_THROW(s3Service.DeleteObject(deleteRequest));

        // assert
        Dto::S3::ListBucketRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.name = TEST_BUCKET;
        const Dto::S3::ListBucketResponse listResponse = s3Service.ListBucket(listRequest);
        BOOST_CHECK_EQUAL(0, listResponse.keyCount);
    }

    BOOST_AUTO_TEST_CASE(ObjectCopyTest) {

        // arrange
        Service::S3Service s3Service;
        CreateDefaultBucket(s3Service);
        PutDefaultObject(s3Service);

        Dto::S3::CopyObjectRequest copyRequest;
        copyRequest.region = TEST_REGION;
        copyRequest.sourceBucket = TEST_BUCKET;
        copyRequest.sourceKey = TEST_KEY;
        copyRequest.targetBucket = TEST_BUCKET;
        copyRequest.targetKey = TEST_COPY_KEY;

        // act
        const Dto::S3::CopyObjectResponse copyResponse = s3Service.CopyObject(copyRequest);

        // assert
        BOOST_CHECK_EQUAL(false, copyResponse.eTag.empty());

        Dto::S3::ListBucketRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.name = TEST_BUCKET;
        const Dto::S3::ListBucketResponse listResponse = s3Service.ListBucket(listRequest);
        BOOST_CHECK_EQUAL(2, listResponse.keyCount);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database
