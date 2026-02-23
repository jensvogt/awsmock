//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_S3_SERVICE_H
#define AWSMOCK_SERVICE_S3_SERVICE_H

// C++ standard includes
#include <string>

#if __APPLE__
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#endif

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/module/mapper/Mapper.h>
#include <awsmock/dto/s3/CompleteMultipartUploadRequest.h>
#include <awsmock/dto/s3/CompleteMultipartUploadResult.h>
#include <awsmock/dto/s3/CopyObjectRequest.h>
#include <awsmock/dto/s3/CopyObjectResponse.h>
#include <awsmock/dto/s3/CreateBucketRequest.h>
#include <awsmock/dto/s3/CreateBucketResponse.h>
#include <awsmock/dto/s3/CreateMultipartUploadRequest.h>
#include <awsmock/dto/s3/CreateMultipartUploadResult.h>
#include <awsmock/dto/s3/DeleteBucketLifecycleRequest.h>
#include <awsmock/dto/s3/DeleteBucketRequest.h>
#include <awsmock/dto/s3/DeleteObjectRequest.h>
#include <awsmock/dto/s3/GetBucketLifecycleConfigurationRequest.h>
#include <awsmock/dto/s3/GetBucketLifecycleConfigurationResponse.h>
#include <awsmock/dto/s3/GetEventSourceRequest.h>
#include <awsmock/dto/s3/GetEventSourceResponse.h>
#include <awsmock/dto/s3/GetMetadataRequest.h>
#include <awsmock/dto/s3/GetMetadataResponse.h>
#include <awsmock/dto/s3/GetObjectRequest.h>
#include <awsmock/dto/s3/GetObjectResponse.h>
#include <awsmock/dto/s3/ListAllBucketResponse.h>
#include <awsmock/dto/s3/ListBucketArnsResponse.h>
#include <awsmock/dto/s3/ListBucketRequest.h>
#include <awsmock/dto/s3/ListBucketResponse.h>
#include <awsmock/dto/s3/ListObjectVersionsRequest.h>
#include <awsmock/dto/s3/ListObjectVersionsResponse.h>
#include <awsmock/dto/s3/MoveObjectRequest.h>
#include <awsmock/dto/s3/MoveObjectResponse.h>
#include <awsmock/dto/s3/PurgeBucketRequest.h>
#include <awsmock/dto/s3/PutBucketEncryptionRequest.h>
#include <awsmock/dto/s3/PutBucketLifecycleConfigurationRequest.h>
#include <awsmock/dto/s3/PutBucketNotificationConfigurationRequest.h>
#include <awsmock/dto/s3/PutBucketNotificationConfigurationResponse.h>
#include <awsmock/dto/s3/PutBucketVersioningRequest.h>
#include <awsmock/dto/s3/PutObjectRequest.h>
#include <awsmock/dto/s3/PutObjectResponse.h>
#include <awsmock/dto/s3/UpdateBucketRequest.h>
#include <awsmock/dto/s3/UploadPartCopyRequest.h>
#include <awsmock/dto/s3/UploadPartCopyResponse.h>
#include <awsmock/dto/s3/internal/DeleteObjectsRequest.h>
#include <awsmock/dto/s3/internal/DeleteObjectsResponse.h>
#include <awsmock/dto/s3/internal/GetBucketRequest.h>
#include <awsmock/dto/s3/internal/GetBucketResponse.h>
#include <awsmock/dto/s3/internal/GetObjectCounterRequest.h>
#include <awsmock/dto/s3/internal/GetObjectCounterResponse.h>
#include <awsmock/dto/s3/internal/ListBucketCounterRequest.h>
#include <awsmock/dto/s3/internal/ListBucketCounterResponse.h>
#include <awsmock/dto/s3/internal/ListObjectCounterRequest.h>
#include <awsmock/dto/s3/internal/ListObjectCounterResponse.h>
#include <awsmock/dto/s3/internal/TouchObjectRequest.h>
#include <awsmock/dto/s3/internal/UpdateObjectRequest.h>
#include <awsmock/dto/s3/internal/UploadObjectCounterRequest.h>
#include <awsmock/dto/s3/mapper/Mapper.h>
#include <awsmock/dto/s3/model/EventNotification.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>
#include <awsmock/entity/s3/LambdaNotification.h>
#include <awsmock/entity/s3/QueueNotification.h>
#include <awsmock/entity/s3/TopicNotification.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/kms/KMSService.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/lambda/LambdaService.h>
#include <awsmock/service/s3/S3HashCreator.h>
#include <awsmock/service/sns/SNSService.h>
#include <awsmock/service/sqs/SQSService.h>
#include <awsmock/service/transfer/TransferService.h>

namespace AwsMock::Service {
    /**
     * @brief S3 service.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3Service {
    public:
        /**
         * @brief Constructor
         */
        explicit S3Service(boost::asio::io_context &ioc) : _database(Database::S3Database::instance()), _lambdaService(ioc) {
        };

        /**
         * @brief Checks whether a bucket exists
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return true if bucket exists
         */
        [[nodiscard]] bool BucketExists(const std::string &region, const std::string &bucket) const;

        /**
         * @brief Returns the metadata of an S3 bucket
         *
         * @param request get metadata request
         * @return GetMetadataResponse
         */
        [[nodiscard]] Dto::S3::GetMetadataResponse GetBucketMetadata(const Dto::S3::GetMetadataRequest &request) const;

        /**
         * @brief Returns the S3 bucket.
         *
         * @par
         * The full bucket, with all attributes and notifications, is returned.
         *
         * @param request get bucket request
         * @return GetBucketResponse
         * @see GetBucketResponse
         */
        [[nodiscard]] Dto::S3::GetBucketResponse GetBucket(const Dto::S3::GetBucketRequest &request) const;

        /**
         * @brief Returns the metadata of an S3 object
         *
         * @param request get metadata request
         * @return GetMetadataResponse
         */
        [[nodiscard]] Dto::S3::GetMetadataResponse GetObjectMetadata(const Dto::S3::GetMetadataRequest &request) const;

        /**
         * @brief Creates a new bucket
         *
         * @param s3Request S3 create request
         * @return CreateBucketResponse
         */
        [[nodiscard]] Dto::S3::CreateBucketResponse CreateBucket(const Dto::S3::CreateBucketRequest &s3Request) const;

        /**
         * @brief Purge a bucket
         *
         * @par
         * This will delete all objects of the given bucket. The bucket itself is
         * still existing, but empty.
         *
         * @param request S3 purge request
         * @return number of objects deleted
         */
        long PurgeBucket(const Dto::S3::PurgeBucketRequest &request) const;

        /**
         * @brief Updates a bucket
         *
         * @param request S3 update request
         */
        void UpdateBucket(const Dto::S3::UpdateBucketRequest &request) const;

        /**
         * @brief Lists all buckets
         *
         * @return ListAllBucketResponse
         */
        [[nodiscard]] Dto::S3::ListAllBucketResponse ListAllBuckets() const;

        /**
         * @brief Lists contents of bucket
         *
         * @param s3Request S3 create request
         * @return CreateBucketResponse
         */
        [[nodiscard]] Dto::S3::ListBucketResponse ListBucket(const Dto::S3::ListBucketRequest &s3Request) const;

        /**
         * @brief Lists bucket counters
         *
         * @param request S3 list bucket counters request
         * @return ListBucketCounterResponse
         */
        [[nodiscard]] Dto::S3::ListBucketCounterResponse ListBucketCounters(const Dto::S3::ListBucketCounterRequest &request) const;

        /**
         * @brief Lists bucket ARNs
         *
         * @return ListBucketArnsResponse
         */
        [[nodiscard]] Dto::S3::ListBucketArnsResponse ListBucketArns() const;

        /**
         * @brief Put bucket versioning
         *
         * @param request S3 put versioning request
         */
        void PutBucketVersioning(const Dto::S3::PutBucketVersioningRequest &request) const;

        /**
         * @brief Put bucket lifecycle configuration
         *
         * @param request S3 put lifecycle configuration request
         */
        void PutBucketLifecycleConfiguration(const Dto::S3::PutBucketLifecycleConfigurationRequest &request) const;

        /**
         * @brief Gets the bucket lifecycle configurations
         *
         * @param request get bucket lifecycle configuration request
         */
        Dto::S3::GetBucketLifecycleConfigurationResponse GetBucketLifecycleConfiguration(const Dto::S3::GetBucketLifecycleConfigurationRequest &request) const;

        void DeleteBucketLifecycle(const Dto::S3::DeleteBucketLifecycleRequest &request) const;

        /**
         * @brief Creates a new bucket
         *
         * @param request S3 multi part upload request
         * @return Dto::S3::CreateMultipartUploadResult
         */
        [[nodiscard]] Dto::S3::CreateMultipartUploadResult CreateMultipartUpload(Dto::S3::CreateMultipartUploadRequest &request) const;

        /**
         * @brief Upload a partial file
         *
         * @param stream input stream
         * @param part part number
         * @param updateId upload ID
         * @param length content length
         * @return ETag
         */
        static std::string UploadPart(std::istream &stream, int part, const std::string &updateId, long length);

        /**
         * @brief Upload a partial file copy.
         *
         * @par
         * This request does not have a body. The header contains the source
         * bucket/key and the range to copy.
         *
         * @param request upload part copy request
         * @return UploadPartCopyResponse
         * @see Dto::S3::UploadPartCopyResponse
         */
        [[nodiscard]] Dto::S3::UploadPartCopyResponse UploadPartCopy(const Dto::S3::UploadPartCopyRequest &request) const;

        /**
         * @brief Completes a multipart upload.
         *
         * @param request multipart upload request
         * @return Dto::S3::CreateMultipartUploadResult
         */
        [[nodiscard]] Dto::S3::CompleteMultipartUploadResult CompleteMultipartUpload(const Dto::S3::CompleteMultipartUploadRequest &request);

        /**
         * @brief Returns an event source as a lambda configuration
         *
         * @param request get event source request
         * @return Dto::S3::GetEventSourceResponse
         */
        [[nodiscard]] Dto::S3::GetEventSourceResponse GetEventSource(const Dto::S3::GetEventSourceRequest &request) const;

        /**
         * @brief Get an object
         *
         * @param request put object request
         * @return GetObjectResponse
         */
        [[nodiscard]] Dto::S3::GetObjectResponse GetObject(const Dto::S3::GetObjectRequest &request) const;

        /**
         * @brief Put an object
         *
         * @param request put object request
         * @param stream input stream
         * @return PutObjectResponse
         */
        Dto::S3::PutObjectResponse PutObject(Dto::S3::PutObjectRequest &request, std::istream &stream);

        /**
         * @brief Put an object, reading a file path
         *
         * @param username
         * @param filename input file
         * @param serverId current server id
         */
        void PutObject(const std::string &username, const std::string &filename, const std::string &serverId);

        /**
         * @brief Touch an object
         *
         * @par
         * This method re-triggers the event notification, like lambda triggers, etc.
         *
         * @param request touch object request
         * @see TouchObjectRequest
         */
        void TouchObject(const Dto::S3::TouchObjectRequest &request);

        /**
         * @brief Update an object
         *
         * @par
         * This method re-triggers the event notification, like lambda trigger etc.
         *
         * @param request touch object request
         * @see TouchObjectRequest
         */
        void UpdateObject(const Dto::S3::UpdateObjectRequest &request) const;

        /**
         * @brief Copy an object
         *
         * @param request copy object request
         * @return PutObjectResponse
         */
        [[nodiscard]] Dto::S3::CopyObjectResponse CopyObject(const Dto::S3::CopyObjectRequest &request);

        /**
         * @brief Move an object
         *
         * @param request move object request
         * @return PutObjectResponse
         */
        [[nodiscard]] Dto::S3::MoveObjectResponse MoveObject(const Dto::S3::MoveObjectRequest &request);

        /**
         * @brief Lists object counters
         *
         * @param s3Request S3 list object counters request
         * @return ListObjectCounterResponse
         */
        [[nodiscard]] Dto::S3::ListObjectCounterResponse ListObjectCounters(const Dto::S3::ListObjectCounterRequest &s3Request) const;

        /**
         * @brief Returns an object counters
         *
         * @param request S3 get object counters request
         * @return GetObjectCounterResponse
         */
        [[nodiscard]] Dto::S3::GetObjectCounterResponse GetObjectCounters(const Dto::S3::GetObjectCounterRequest &request) const;

        void UploadObjectCounter(const Dto::S3::UploadObjectCounterRequest &request);

        /**
         * @brief Uploads a S3 object
         *
         * @param request S3 upload object counter request
         */
        void UploadObjectCounter(const Dto::S3::UploadObjectCounterRequest &request) const;

        /**
         * @brief Delete object
         *
         * @param request delete object request
         */
        void DeleteObject(const Dto::S3::DeleteObjectRequest &request);

        /**
         * @brief Delete objects
         *
         * @param request delete objects request
         * @return DeleteObjectsResponse
         */
        [[nodiscard]] Dto::S3::DeleteObjectsResponse DeleteObjects(const Dto::S3::DeleteObjectsRequest &request);

        /**
         * @brief Adds a bucket notification configuration
         *
         * @param request bucket notification configuration request.
         * @return PutBucketNotificationConfigurationResponse,
         * @see PutBucketNotificationConfigurationRequest
         * @see PutBucketNotificationConfigurationResponse
         */
        [[nodiscard]] Dto::S3::PutBucketNotificationConfigurationResponse PutBucketNotificationConfiguration(const Dto::S3::PutBucketNotificationConfigurationRequest &request) const;

        /**
         * @brief Adds a bucket encryption configuration
         *
         * @param request bucket encryption configuration request.
         * @see PutBucketEncryptionRequest
         */
        void PutBucketEncryption(const Dto::S3::PutBucketEncryptionRequest &request) const;

        /**
         * @brief Returns a list object versions
         *
         * @param request list object versions request
         * @return ListObjectVersionsResponse
         * @see AwsMock::Dto::S3::ListObjectVersionsRequest
         * @see AwsMock::Dto::S3::ListObjectVersionsResponse
         */
        [[nodiscard]] Dto::S3::ListObjectVersionsResponse ListObjectVersions(const Dto::S3::ListObjectVersionsRequest &request) const;

        /**
         * @brief Delete a bucket
         *
         * @param request bucket delete request.
         */
        void DeleteBucket(const Dto::S3::DeleteBucketRequest &request) const;

    private:
        /**
         * @brief Sends a message to the corresponding SQS queue.
         *
         * @param eventNotification S3 event notification.
         * @param queueNotification queue notification.
         */
        void SendQueueNotificationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::QueueNotification &queueNotification);

        /**
         * @brief Sends a message to the corresponding SNS topic.
         *
         * @param eventNotification S3 event notification.
         * @param topicNotification topic notification.
         */
        void SendTopicNotificationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::TopicNotification &topicNotification);

        /**
         * @brief Send lambda function invocation request to lambda module.
         *
         * <p>This will send a lambda invocation request to the lambda module. The
         * lambda module will StartServer the corresponding lambda function and will
         * send the S3 notification request to the lambda function.</p>
         *
         * @param eventNotification S3 event notification
         * @param lambdaNotification S3 lambda notification
         */
        void SendLambdaInvocationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::LambdaNotification &lambdaNotification) const;

        /**
         * @brief Check for bucket notifications.
         *
         * @param region AWS region.
         * @param bucket bucket name.
         * @param key S3 object key.
         * @param size S3 object size in bytes.
         * @param event S3 event type.
         */
        void CheckNotifications(const std::string &region, const std::string &bucket, const std::string &key, long size, const std::string &event);

        /**
         * @brief Checks the encryption status and encrypt the internal file using the
         * KMS key supplied in the encryption object of the bucket.
         *
         * @param bucket S3 bucket
         * @param object S3 object
         */
        static void CheckEncryption(const Database::Entity::S3::Bucket &bucket, const Database::Entity::S3::Object &object);

        /**
         * @brief Checks the existence of a bucket by region and name.
         *
         * @param region AWS region
         * @param name S3 bucket name
         */
        static void CheckBucketExistence(const std::string &region, const std::string &name);

        /**
         * @brief Checks whether the bucket exists already.
         *
         * @param region AWS region
         * @param name S3 bucket name
         */
        static void CheckBucketNonExistence(const std::string &region, const std::string &name);

        /**
         * @brief Checks the decryption status and decrypts the internal file using
         * the KMS key supplied in the encryption object of the bucket.
         *
         * <p>
         * The decrypted file will be written to outFile. This file needs to be
         * deleted, after the file has been send back to the client.
         * </p>
         *
         * @param bucket S3 bucket
         * @param object S3 object
         * @param outFile name of the output file
         */
        static void CheckDecryption(const Database::Entity::S3::Bucket &bucket, const Database::Entity::S3::Object &object, std::string &outFile);

        /**
         * @brief Get the temporary upload directory for a uploadId.
         *
         * @param uploadId S3 multipart upload ID
         * @return temporary directory path.
         */
        static std::string GetMultipartUploadDirectory(const std::string &uploadId);

        /**
         * @brief Deletes an object
         *
         * @param bucket S3 bucket
         * @param key S3 object key
         * @param internalName S3 internal name
         */
        static void DeleteObject(const std::string &bucket, const std::string &key, const std::string &internalName);

        /**
         * @brief Deletes a bucket
         *
         * @par
         * This method is recursive, if the bucket contains objects, also all objects are removed.
         *
         * @param bucket S3 bucket name
         */
        static void DeleteBucket(const std::string &bucket);

        /**
         * @brief Save a versioned S3 object.
         *
         * @param request put object request
         * @param bucket S3 bucket
         * @param stream input stream
         * @return file name
         */
        Dto::S3::PutObjectResponse SaveVersionedObject(Dto::S3::PutObjectRequest &request, const Database::Entity::S3::Bucket &bucket, std::istream &stream);

        /**
         * @brief Save an un-versioned S3 object.
         *
         * @param request put object request
         * @param bucket S3 bucket
         * @param stream input stream
         * @param size input stream size
         * @return file name
         */
        Dto::S3::PutObjectResponse SaveUnversionedObject(Dto::S3::PutObjectRequest &request, const Database::Entity::S3::Bucket &bucket, std::istream &stream, long size);

        /**
         * @brief Adds the queue notification configuration to the provided bucket.
         *
         * @param bucket bucket entity.
         * @param queueConfigurations queue notification configurations vector.
         */
        static void PutQueueNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::QueueConfiguration> &queueConfigurations);

        /**
         * @brief Adds the topic notification configuration to the provided bucket.
         *
         * @param bucket bucket entity.
         * @param topicConfigurations topic notification configurations vector.
         */
        static void PutTopicNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::TopicConfiguration> &topicConfigurations);

        /**
         * @brief Adds the lambda notification configuration to the provided bucket.
         *
         * @param bucket bucket entity.
         * @param lambdaConfigurations lambda notification configurations vector.
         */
        void PutLambdaNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::LambdaConfiguration> &lambdaConfigurations) const;

        /**
         * @brief Adds the lambda notification configuration to the provided bucket.
         *
         * @param contentType content type from AWS S3.
         * @param filePath file path
         * @param s3Key S3 key
         */
        static std::string SanitizeContentType(const std::string &contentType, const std::string &filePath, const std::string &s3Key);

        /**
         * Database connection
         */
        Database::S3Database &_database;

        /**
         * Lambda service
         */
        LambdaService _lambdaService;

        /**
         * IO context
         */
        boost::asio::io_context _ioc;
    };
} // namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_S3_SERVICE_H
