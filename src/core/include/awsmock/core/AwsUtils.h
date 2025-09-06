//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWS_MOCK_CORE_AWS_UTILS_H
#define AWS_MOCK_CORE_AWS_UTILS_H

// C++ standard includes
#include <chrono>
#include <iostream>
#include <string>

// Boost includes
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/UnauthorizedException.h>
#include <awsmock/core/logging/LogStream.h>

#define S3_FILE_NAME_LENGTH 64
#define S3_VERSION_ID_LENGTH 64

#define SQS_RECEIPT_HANDLE_LENGTH 256
#define SQS_DEFAULT_ACCOUNT_ID "000000000000"
#define SQS_DEFAULT_REGION "eu-central-1"

#define SNS_RECEIPT_HANDLE_LENGTH 256

#define GATEWAY_DEFAULT_PORT 4566
#define GATEWAY_DEFAULT_PROTOCOL std::string("http")
#define GATEWAY_DEFAULT_REGION "eu-central-1"

namespace AwsMock::Core {

    struct AuthorizationHeaderKeys {

        /**
         * AWS signing version, i.e: AWS4-HMAC-SHA256
         */
        std::string signingVersion;

        /**
         * Secret access key
         */
        std::string secretAccessKey;

        /**
         * Date time
         */
        std::string dateTime;

        /**
         * Date time
         */
        std::string isoDateTime;

        /**
         * Scope
         */
        std::string scope;

        /**
         * Region
         */
        std::string region;

        /**
         * Module
         */
        std::string module;

        /**
         * Request version
         */
        std::string requestVersion;

        /**
         * Signed headers
         */
        std::string signedHeaders;

        /**
         * Signature
         */
        std::string signature;
    };

    /**
     * @brief General AWS utilities.
     *
     * AWS utilities
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AwsUtils {

      public:

        /**
         * @brief Create S3 Amazon bucket ARN
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param bucket bucket name
         * @return AWS resource ARN
         */
        static std::string CreateS3BucketArn(const std::string &region, const std::string &accountId, const std::string &bucket);

        /**
         * @brief Create S3 Amazon bucket/key ARN
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param bucket bucket name
         * @param key object key
         * @return AWS resource ARN
         */
        static std::string CreateS3ObjectArn(const std::string &region, const std::string &accountId, const std::string &bucket, const std::string &key);

        /**
         * @brief Create lambda function ARN
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param function lambda function name
         * @return AWS resource ARN
         */
        static std::string CreateLambdaArn(const std::string &region, const std::string &accountId, const std::string &function);

        /**
         * @brief Creates a queue ARN
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param queueName name of the queue
         * @return AWS resource ARN
         */
        static std::string CreateSQSQueueArn(const std::string &region, const std::string &accountId, const std::string &queueName);

        /**
         * @brief Creates an API key ID
         *
         * @return API key ID
         */
        static std::string CreateApiKeyId();

        /**
         * @brief Creates a REST API ID
         *
         * @return REST API ID
         */
        static std::string CreateRestApiId();

        /**
         * @brief Creates an API gateway API key
         *
         * @return API key
         */
        static std::string CreateApiGatewayApiKey();

        /**
         * @brief Converts a queue ARN to a queue URL
         *
         * @param queueArn ARN of the queue
         * @return URL of the queue
         */
        static std::string ConvertSQSQueueArnToUrl(const std::string &queueArn);

        /**
         * @brief Converts a queue ARN to a queue name
         *
         * @param queueArn ARN of the queue
         * @return name of the queue
         */
        static std::string ConvertSQSQueueArnToName(const std::string &queueArn);

        /**
         * @brief Converts a queue ARN to a queue URL
         *
         * @param region AWS region
         * @param queueUrl URL of the queue
         * @return URL of the queue
         */
        static std::string ConvertSQSQueueUrlToArn(const std::string &region, const std::string &queueUrl);

        /**
         * @brief Converts a queue URL to a queue name
         *
         * @param queueUrl URL of the queue
         * @return name of the queue
         */
        static std::string ConvertSQSQueueUrlToName(const std::string &queueUrl);

        /**
         * @brief Creates a queue ARN
         *
         * @return SQS sender ID
         */
        static std::string CreateSQSSenderId();

        /**
         * @brief  Create SNS topic ARN
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param topicName name of the topic
         * @return SNS topic ARN
         */
        static std::string CreateSNSTopicArn(const std::string &region, const std::string &accountId, const std::string &topicName);

        /**
         * @brief Create SNS subscription ARN
         *
         * <p>Syntax arn:aws:sns:us-west-2:123456789012:MyTopic:6b0e71bd-7e97-4d97-80ce-4a0994e55286</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param topicName name of the topic
         * @return SNS topic subscription ARN
         */
        static std::string CreateSNSSubscriptionArn(const std::string &region, const std::string &accountId, const std::string &topicName);

        /**
         * @brief Create SNS subscription ARN
         *
         * <p>Syntax arn:aws:sns:us-west-2:123456789012:MyTopic:6b0e71bd-7e97-4d97-80ce-4a0994e55286</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param name parameter name
         * @return SNS topic subscription ARN
         */
        static std::string CreateSSMParameterArn(const std::string &region, const std::string &accountId, const std::string &name);

        /**
         * @brief Create transfer manager ARN
         *
         * <p>Syntax arn:aws:sns:us-west-2:123456789012:serverId</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param serverId ID of the transfer manager
         * @return transfer server ARN
         */
        static std::string CreateTransferArn(const std::string &region, const std::string &accountId, const std::string &serverId);

        /**
         * @brief Create transfer manager ARN
         *
         * <p>Syntax arn:aws:sns:us-west-2:123456789012:userName</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param serverId ID of the transfer manager
         * @param userName user name
         * @return transfer user ARN
         */
        static std::string CreateTransferUserArn(const std::string &region, const std::string &accountId, const std::string &serverId, const std::string &userName);

        /**
         * @brief Create secrets manager ARN
         *
         * <p>Syntax arn:aws:secrets manager:us-west-2:123456789012:secret:MyDatabaseSecretName-a1b2c3</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param secretId ID of the secret (name + 6 hex digits)
         * @return secrets manager secrets ARN
         */
        static std::string CreateSecretArn(const std::string &region, const std::string &accountId, const std::string &secretId);

        /**
         * @brief Create KMS key ARN
         *
         * <p>Syntax arn:aws:kms:us-west-2:123456789012:name</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param kmsId ID of the key
         * @return KMS key ARN
         */
        static std::string CreateKMSKeyArn(const std::string &region, const std::string &accountId, const std::string &kmsId);

        /**
         * @brief Create DynamoDB table ARN
         *
         * <p>Syntax arn:aws:dynamodb:ddblocal:123456789012:table/tableName</p>
         *
         * @param accountId AWS account ID
         * @param tableName table name
         * @return DynamoDb table ARN
         */
        static std::string CreateDynamoDbTableArn(const std::string &accountId, const std::string &tableName);

        /**
         * @brief Create Cognito user pool ID
         *
         * <p>region_<9-random-digit></p>
         *
         * @param region AWS region
         * @return ID of the user pool
         */
        static std::string CreateCognitoUserPoolId(const std::string &region);

        /**
         * @brief Create Cognito user pool ARN
         *
         * <p>Syntax arn:aws:cognito:us-west-2:123456789012:userPoolId</p>
         *
         * @param region AWS region
         * @param accountId AWS account ID
         * @param userPoolId ID of the user pool
         * @return user pool ARN
         */
        static std::string CreateCognitoUserPoolArn(const std::string &region, const std::string &accountId, const std::string &userPoolId);

        /**
         * @brief Creates a new Cognito confirmation code
         *
         * @return Cognito confirmation code
         */
        static std::string CreateCognitoConfirmationCode();

        /**
         * @brief Returns a request ID.
         *
         * @return request ID
         */
        static std::string CreateRequestId();

        /**
         * @brief Returns a message ID.
         *
         * <p>Used for SQS and SNS to generate a message ID, which is basically a UUID.</p>
         *
         * @return message ID
         */
        static std::string CreateMessageId();

        /**
         * @brief Returns a receipt handle.
         *
         * @return receipt handle
         */
        static std::string CreateSqsReceiptHandler() {
            return StringUtils::GenerateRandomString(SQS_RECEIPT_HANDLE_LENGTH);
        }

        /**
         * @brief Returns a receipt handle.
         *
         * @return receipt handle
         */
        static std::string CreateSnsReceiptHandler() {
            return StringUtils::GenerateRandomString(SNS_RECEIPT_HANDLE_LENGTH);
        }

        /**
         * @brief Generate a S3 file name
         *
         * @return random file name
         */
        static std::string CreateS3FileName() {
            return StringUtils::GenerateRandomHexString(S3_FILE_NAME_LENGTH);
        }

        /**
         * @brief Generate a S3 version ID
         *
         * @return random version ID
         */
        static std::string CreateS3VersionId() {
            return StringUtils::GenerateRandomVersion(S3_VERSION_ID_LENGTH);
        }

        /**
         * @brief Add the AWS v4 signed authorization header
         *
         * Adds the AWS authorization header, based on the supplied Poco HTTP request.
         *
         * @param request HTTP request
         * @param module AwsMock module
         * @param contentType HTTP content type
         * @param signedHeaders signed header names
         * @param payload HTTP payload
         */
        static void AddAuthorizationHeader(http::request<http::dynamic_body> &request, const std::string &module, const std::string &contentType, const std::string &signedHeaders, const std::string &payload);

        /**
         * @brief Add the AWS v4 signed authorization header
         *
         * Adds the AWS authorization header, based on the supplied Curl request HTTP request.
         *
         * @param method HTTP method
         * @param path HTTP path
         * @param headers HTTP headers
         * @param module AwsMock module
         * @param contentType HTTP content type
         * @param signedHeaders signed header names
         * @param payload HTTP payload
         */
        static void AddAuthorizationHeader(const std::string &method, const std::string &path, std::map<std::string, std::string> &headers, const std::string &module, const std::string &contentType, const std::string &signedHeaders, const std::string &payload);

        /**
         * @brief Verify the request signature
         *
         * Examples:
         * https://github.com/aws-samples/sigv4-signing-examples
         * https://docs.aws.amazon.com/IAM/latest/UserGuide/create-signed-request.html#code-signing-examples
         * https://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return true if signature could be verified
         */
        static bool VerifySignature(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey);

        /**
         * @brief Verify the request signature
         *
         * Examples:
         * https://github.com/aws-samples/sigv4-signing-examples
         * https://docs.aws.amazon.com/IAM/latest/UserGuide/create-signed-request.html#code-signing-examples
         * https://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return true if signature could be verified
         */
        static bool VerifySignature(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &secretAccessKey);

        /**
         * @brief Returns the bucket name from the request.
         *
         * @par
         * Distinguishes between host-style and path-style requests.
         *
         * @param request HTTP request
         * @return bucket name.
         */
        static std::string GetS3BucketName(const http::request<http::dynamic_body> &request);

        /**
         * @brief Returns the object key name from the request.
         *
         * @par
         * Distinguishes between host-style and path-style requests.
         *
         * @param request HTTP request
         * @return bucket name.
         */
        static std::string GetS3ObjectKey(const http::request<http::dynamic_body> &request);

        /**
         * @brief Checks whether this is a host-style or path-style S3 request
         *
         * @param request HTTP request.
         */
        static bool IsS3HostStyle(const http::request<http::dynamic_body> &request);

        /**
          * @brief Returns the bucket name for a S3 host-style request
          *
          * @param request HTTP request
          * @return bucket name
          */
        static std::string GetS3HostStyleBucket(const http::request<http::dynamic_body> &request);

        /**
          * @brief Returns the bucket name for a S3 path-style request
          *
          * @param request HTTP request
          * @return bucket name
          */
        static std::string GetS3PathStyleBucket(const http::request<http::dynamic_body> &request);

        /**
          * @brief Returns the object key for a S3 host-style request
          *
          * @param request HTTP request
          * @return object key
          */
        static std::string GetS3HostStyleObjectKey(const http::request<http::dynamic_body> &request);

        /**
          * @brief Returns the bucket name for a S3 path-style request
          *
          * @param url HTTP url
          * @return object key
          */
        static std::string GetS3PathStyleObjectKey(const std::string &url);

        /**
         * @brief Extracts the bucket name of an ARN
         *
         * @param arn S3 resource ARN
         * @return bucket name
         */
        static std::string GetBucketNameFromArn(const std::string &arn) {
            return arn.substr(arn.find_last_of(":") + 1);
        }

      private:

        /**
         * @brief Returns the canonical request.
         *
         * @param request HTTP request
         * @param authorizationHeaderKeys
         * @return
         */
        static std::string GetCanonicalRequest(const http::request<http::dynamic_body> &request, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Returns the canonical request.
         *
         * @param request HTTP request
         * @param authorizationHeaderKeys
         * @return
         */
        static std::string GetCanonicalRequest(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Returns the canonical request.
         *
         * This is used by the Curl base request.
         *
         * @param method HTTP method
         * @param path HTTP path, including query/path parameter
         * @param headers HTTP headers
         * @param payload HTTP payload
         * @param authorizationHeaderKeys
         * @return
         */
        static std::string GetCanonicalRequest(const std::string &method, const std::string &path, std::map<std::string, std::string> &headers, const std::string &payload, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Creates the string to sign.
         *
         * @param canonicalRequest canonical request
         * @param authorizationHeaderKeys authorization summary
         * @return string to sign
         */
        static std::string GetStringToSign(const std::string &canonicalRequest, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Returns the canonical query parameters
         *
         * This is used by the Curl base request.
         *
         * @param path HTTP query path
         * @return canonical request parameters
         */
        static std::string GetCanonicalQueryParameters(const std::string &path);

        /**
         * @brief Returns the canonical header string.
         *
         * @param request HTTP request
         * @param authorizationHeaderKeys authorization parameter
         * @return canonical header string
         */
        static std::string GetCanonicalHeaders(const http::request<http::dynamic_body> &request, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Returns the canonical headers.
         *
         * @param request
         * @param authorizationHeaderKeys
         * @return list of canonical headers
         */
        static std::string GetCanonicalHeaders(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Returns the canonical header string.
         *
         * This is used by the Curl base request.
         *
         * @param headers HTTP headers
         * @param authorizationHeaderKeys authorization parameter
         * @return canonical header string
         */
        static std::string GetCanonicalHeaders(std::map<std::string, std::string> &headers, const AuthorizationHeaderKeys &authorizationHeaderKeys);

        /**
         * @brief Returns the hashed payload.
         *
         * @param payload HTTP payload
         * @return hashed payload
         */
        static std::string GetHashedPayload(const std::string &payload);

        /**
         * @brief Splits the authorization header into pieces and stores the result into a struct.
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return AuthorizationHeaderKeys
         */
        static AuthorizationHeaderKeys GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey);

        /**
         * @brief Splits the authorization header into pieces and stores the result into a struct.
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return AuthorizationHeaderKeys
         */
        static AuthorizationHeaderKeys GetAuthorizationKeys(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &secretAccessKey);

        /**
         * @brief Returns the signature for the request
         *
         * @param authorizationHeaderKeys authorization header
         * @param stringToSign string to sign
         * @return key encoded signature
         */
        static std::string GetSignature(const AuthorizationHeaderKeys &authorizationHeaderKeys, const std::string &stringToSign);

        /**
         * @brief Returns the date string in format yyyyMMdd
         *
         * Return the time in zone UTC
         *
         * @return date string.
         */
        static std::string GetDateString();

        /**
         * @brief Returns the date time string in ToISO8601 format
         *
         * Return the time in zone UTC
         *
         * @return date string.
         */
        static std::string GetISODateString();
    };

    /**
     * @brief Returns the HTTP endpoint
     *
     * @return HTTP endpoint
     */
    inline std::string GetEndpoint() {
        const int port = Configuration::instance().GetValue<int>("awsmock.gateway.http.port");
        const std::string hostname = SystemUtils::GetHostName();
        return GATEWAY_DEFAULT_PROTOCOL + "://" + hostname + ":" + std::to_string(port);
    }

    /**
     * @brief Create Amazon Resource Names (ARNs)
     *
     * @param service AWS module name
     * @param region AWS region
     * @param accountId AWS accountId
     * @param resourceId AWS resourceId
     */
    inline std::string CreateArn(const std::string &service, const std::string &region, const std::string &accountId, const std::string &resourceId) {
        return "arn:aws:" + service + ":" + region + ":" + accountId + ":" + resourceId;
    }

    /**
     * @brief Generate a SQS queue URL.
     *
     * The queue name has the format <i>'http://sqs.&lt;region&gt;.&lt;host&lt;port&gt;/&lt;accountId&gt;/&lt;queueName&gt;'</i>
     *
     * @param queueName name of the queue
     * @return SQS queue URL
     */
    inline std::string CreateSQSQueueUrl(const std::string &queueName) {
        const std::string hostname = SystemUtils::GetHostName();
        const auto port = Configuration::instance().GetValue<std::string>("awsmock.gateway.http.port");
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const auto queueUrl = "http://sqs." + region + "." + hostname + ":" + port + "/" + accountId + "/" + queueName;
        log_trace << "queueUrl: " << queueUrl;
        return queueUrl;
    }

    /**
     * @brief Create SQS queue ARN
     *
     * @param queueName name of the queue
     * @return SQS queue ARN
     */
    inline std::string CreateSQSQueueArn(const std::string &queueName) {
        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        log_trace << "Region: " << region << " accountId: " << accountId;
        return CreateArn("sqs", region, accountId, queueName);
    }

    /**
     * @brief Create SQS queue ARN
     *
     * @param region AWS region
     * @param queueName name of the queue
     * @return SQS queue ARN
     */
    inline std::string CreateSQSQueueArn(const std::string &region, const std::string &queueName) {
        const std::string accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        log_trace << "Region: " << region << " accountId: " << accountId;
        return CreateArn("sqs", region, accountId, queueName);
    }

    /**
     * @brief Get the name from a queue URL
     *
     * @param queueUrl URL of the queue
     * @return SQS queue ARN
     */
    inline std::string GetSQSQueueNameFromUrl(const std::string &queueUrl) {
        return queueUrl.substr(queueUrl.find_last_of('/'));
    }

    /**
     * Checks whether this is a queue URL
     *
     * @param queue queue from Spring cloud
     * @return true if this is a URL
     */
    inline bool IsSQSUrl(const std::string &queue) {
        return StringUtils::StartsWith(queue, "http");
    }

    /**
     * Checks whether this is a queue ARN
     *
     * @param queue queue from Spring cloud
     * @return true if this is a URL
     */
    inline bool IsSQSArn(const std::string &queue) {
        return StringUtils::ContainsIgnoreCase(queue, "arn");
    }

    /**
     * @brief Spring cloud sends the queueName, the queueUrl, or the queueArn.
     *
     * @param queue from Spring cloud request
     * @return queue URL
     */
    inline std::string SanitizeSQSUrl(const std::string &queue) {
        if (IsSQSUrl(queue)) {
            return queue;
        }
        if (IsSQSArn(queue)) {
            return AwsUtils::ConvertSQSQueueArnToUrl(queue);
        }
        return CreateSQSQueueUrl(queue);
    }

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_AWS_UTILS_H
