//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_S3_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_S3_CLIENT_COMMAND_H

// C++ includes
#include <sstream>
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    enum class S3CommandType {
        CREATE_BUCKET,
        LIST_BUCKETS,
        LIST_BUCKET_COUNTERS,
        GET_BUCKET,
        PURGE_BUCKET,
        UPDATE_BUCKET,
        DELETE_BUCKET,
        LIST_OBJECTS,
        LIST_OBJECT_COUNTERS,
        PUT_OBJECT,
        GET_OBJECT,
        GET_OBJECT_RANGE,
        COPY_OBJECT,
        MOVE_OBJECT,
        DELETE_OBJECT,
        DELETE_OBJECTS,
        CREATE_MULTIPART_UPLOAD,
        UPLOAD_PART,
        UPLOAD_PART_COPY,
        COMPLETE_MULTIPART_UPLOAD,
        ABORT_MULTIPART_UPLOAD,
        LIST_OBJECT_VERSIONS,
        BUCKET_NOTIFICATION,
        PUT_BUCKET_NOTIFICATION_CONFIGURATION,
        PUT_BUCKET_ENCRYPTION,
        PUT_BUCKET_VERSIONING,
        UNKNOWN
    };

    static std::map<S3CommandType, std::string> S3CommandTypeNames{
            {S3CommandType::CREATE_BUCKET, "CreateBucket"},
            {S3CommandType::LIST_BUCKETS, "ListBuckets"},
            {S3CommandType::LIST_BUCKET_COUNTERS, "ListBucketCounters"},
            {S3CommandType::GET_BUCKET, "GetBucket"},
            {S3CommandType::PURGE_BUCKET, "PurgeBucket"},
            {S3CommandType::UPDATE_BUCKET, "UpdateBucket"},
            {S3CommandType::DELETE_BUCKET, "DeleteBucket"},
            {S3CommandType::LIST_OBJECTS, "ListObjects"},
            {S3CommandType::LIST_OBJECT_COUNTERS, "ListObjectCounters"},
            {S3CommandType::PUT_OBJECT, "PutObject"},
            {S3CommandType::GET_OBJECT, "GetObject"},
            {S3CommandType::GET_OBJECT_RANGE, "GetObjectRange"},
            {S3CommandType::COPY_OBJECT, "CopyObject"},
            {S3CommandType::MOVE_OBJECT, "MoveObject"},
            {S3CommandType::DELETE_OBJECT, "DeleteObject"},
            {S3CommandType::DELETE_OBJECTS, "DeleteObjects"},
            {S3CommandType::CREATE_MULTIPART_UPLOAD, "CreateMultipartUpload"},
            {S3CommandType::UPLOAD_PART, "PartMultipartUpload"},
            {S3CommandType::UPLOAD_PART_COPY, "PartMultipartUploadCopy"},
            {S3CommandType::COMPLETE_MULTIPART_UPLOAD, "CompleteMultipartUpload"},
            {S3CommandType::ABORT_MULTIPART_UPLOAD, "AbortMultipartUpload"},
            {S3CommandType::LIST_OBJECT_VERSIONS, "ListObjectVersions"},
            {S3CommandType::BUCKET_NOTIFICATION, "BucketNotification"},
            {S3CommandType::PUT_BUCKET_NOTIFICATION_CONFIGURATION, "PUT_BUCKET_NOTIFICATION_CONFIGURATION"},
            {S3CommandType::PUT_BUCKET_ENCRYPTION, "PUT_BUCKET_ENCRYPTION"},
            {S3CommandType::PUT_BUCKET_VERSIONING, "PUT_BUCKET_VERSIONING"},
    };

    [[maybe_unused]] static std::string S3CommandTypeToString(S3CommandType commandType) {
        return S3CommandTypeNames[commandType];
    }

    [[maybe_unused]] static S3CommandType S3CommandTypeFromString(const std::string &commandType) {
        for (auto &it: S3CommandTypeNames) {
            if (Core::StringUtils::StartsWith(commandType, it.second)) {
                return it.first;
            }
        }
        return S3CommandType::UNKNOWN;
    }

    /**
     * @brief S3 client command
     *
     * The S3 client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a
     * different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct S3ClientCommand : public BaseClientCommand {

        /**
         * Client command
         */
        S3CommandType command;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Key
         */
        std::string prefix;

        /**
         * Versioning
         */
        bool versionRequest = false;

        /**
         * Notification
         */
        bool notificationRequest = false;

        /**
         * Multipart upload/download
         */
        bool multipartRequest = false;

        /**
         * Multipart uploads
         */
        bool uploads = false;

        /**
         * Multipart upload part copy
         */
        bool uploadPartCopy = false;

        /**
         * Multipart part number
         */
        bool partNumber = false;

        /**
         * Multipart upload/download
         */
        bool copyRequest = false;

        /**
         * SSE encryption
         */
        bool encryptionRequest = false;

        /**
         * Get range request
         */
        bool rangeRequest = false;

        /**
         * Multipart upload ID
         */
        std::string uploadId;

        /**
         * Requester host
         */
        std::string host;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Gets command type from the user agent
         *
         * @param httpMethod HTTP request method
         * @param userAgent HTTP user agent
         */
        void GetCommandFromUserAgent(const http::verb &httpMethod, const UserAgent &userAgent);

        /**
         * Getś the value from the user-agent string
         *
         * @param request HTTP server request
         * @param region AWS region
         * @param user AWS user
         */
        void FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const S3ClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_S3_CLIENT_COMMAND_H
