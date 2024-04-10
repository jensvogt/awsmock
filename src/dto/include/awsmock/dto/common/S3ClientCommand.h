//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_S3_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_S3_CLIENT_COMMAND_H

// C++ includes
#include <string>
#include <sstream>

// Poco includes
#include <Poco/RegularExpression.h>
#include <Poco/Net/HTTPServerRequest.h>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/HttpMethod.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

  enum class S3CommandType {
    CREATE_BUCKET,
    LIST_BUCKETS,
    DELETE_BUCKET,
    LIST_OBJECTS,
    PUT_OBJECT,
    GET_OBJECT,
    COPY_OBJECT,
    MOVE_OBJECT,
    DELETE_OBJECT,
    DELETE_OBJECTS,
    CREATE_MULTIPART_UPLOAD,
    UPLOAD_PART,
    COMPLETE_MULTIPART_UPLOAD,
    UNKNOWN
  };

  static std::map<S3CommandType, std::string> S3CommandTypeNames {
    {S3CommandType::CREATE_BUCKET, "CreateBucket"},
    {S3CommandType::LIST_BUCKETS, "ListBuckets"},
    {S3CommandType::DELETE_BUCKET, "DeleteBucket"},
    {S3CommandType::LIST_OBJECTS, "ListObjects"},
    {S3CommandType::PUT_OBJECT, "PutObject"},
    {S3CommandType::GET_OBJECT, "GetObject"},
    {S3CommandType::COPY_OBJECT, "CopyObject"},
    {S3CommandType::MOVE_OBJECT, "MoveObject"},
    {S3CommandType::DELETE_OBJECT, "DeleteObject"},
    {S3CommandType::DELETE_OBJECTS, "DeleteObjects"},
    {S3CommandType::CREATE_MULTIPART_UPLOAD, "CreateMultipartUpload"},
    {S3CommandType::UPLOAD_PART, "PartMultipartUpload"},
    {S3CommandType::COMPLETE_MULTIPART_UPLOAD, "CompleteMultipartUpload"},
  };

  [[maybe_unused]]static std::string S3CommandTypeToString(S3CommandType commandType) {
    return S3CommandTypeNames[commandType];
  }

  [[maybe_unused]]static S3CommandType S3CommandTypeFromString(const std::string &commandType) {
    for (auto &it : S3CommandTypeNames) {
      if (Core::StringUtils::StartsWith(commandType, it.second)) {
        return it.first;
      }
    }
    return S3CommandType::UNKNOWN;
  }

  /**
   * The S3 client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a different way of calling the AWS REST services.
   *
   * @author jens.vogt@opitz-consulting.com
   */
  struct S3ClientCommand {

    /**
     * HTTP request type
     */
    HttpMethod method;

    /**
     * Client region
     */
    std::string region;

    /**
     * Client user
     */
    std::string user;

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
     * Versioning
     */
    bool versionRequest;

    /**
     * Notification
     */
    bool notificationRequest;

    /**
     * Multipart upload/download
     */
    bool multipartRequest;

    /**
     * Multipart upload/download
     */
    bool copyRequest;

    /**
     * Multipart upload ID
     */
    std::string uploadId;

    /**
     * Gets command type from the user agent
     *
     * @param httpMethod HTTP request method
     * @param userAgent HTTP user agent
     */
    void GetCommandFromUserAgent(const HttpMethod &httpMethod, const UserAgent &userAgent);

    /**
     * Getś the value from the user-agent string
     *
     * @param method HTTP method
     * @param request HTTP server request
     * @param region AWS region
     * @param user AWS user
     */
    void FromRequest(const HttpMethod &method, Poco::Net::HTTPServerRequest &request, const std::string &region, const std::string &user);

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

} // namespace AwsMock::Dto::Common

#endif // AWSMOCK_DTO_COMMON_S3_CLIENT_COMMAND_H