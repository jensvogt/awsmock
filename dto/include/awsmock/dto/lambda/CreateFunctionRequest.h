//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/lambda/model/Code.h>
#include <awsmock/dto/lambda/model/Environment.h>
#include <awsmock/dto/lambda/model/EphemeralStorage.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create function response
     *
     * Example:
     * @code{.json}
     * {
     *   "Architectures": [ "string" ],
     *   "Code": {
     *      "ImageUri": "string",
     *      "S3Bucket": "string",
     *      "S3Key": "string",
     *      "S3ObjectVersion": "string",
     *      "ZipFile": blob
     *   },
     *   "CodeSigningConfigArn": "string",
     *   "DeadLetterConfig": {
     *      "TargetArn": "string"
     *   },
     *   "Description": "string",
     *   "Environment": {
     *      "Variables": {
     *         "string" : "string"
     *      }
     *   },
     *   "EphemeralStorage": {
     *      "Size": number
     *   },
     *   "FileSystemConfigs": [
     *      {
     *         "Arn": "string",
     *         "LocalMountPath": "string"
     *      }
     *   ],
     *   "FunctionName": "string",
     *   "Handler": "string",
     *   "ImageConfig": {
     *      "Command": [ "string" ],
     *      "EntryPoint": [ "string" ],
     *      "WorkingDirectory": "string"
     *   },
     *   "KMSKeyArn": "string",
     *   "Layers": [ "string" ],
     *   "LoggingConfig": {
     *      "ApplicationLogLevel": "string",
     *      "LogFormat": "string",
     *      "LogGroup": "string",
     *      "SystemLogLevel": "string"
     *   },
     *   "MemorySize": number,
     *   "PackageType": "string",
     *   "Publish": boolean,
     *   "Role": "string",
     *   "Runtime": "string",
     *   "SnapStart": {
     *      "ApplyOn": "string"
     *   },
     *   "Tags": {
     *      "string" : "string"
     *   },
     *   "Timeout": number,
     *   "TracingConfig": {
     *      "Mode": "string"
     *   },
     *   "VpcConfig": {
     *      "Ipv6AllowedForDualStack": boolean,
     *      "SecurityGroupIds": [ "string" ],
     *      "SubnetIds": [ "string" ]
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateFunctionRequest final : Common::BaseDto<CreateFunctionRequest> {

        /**
         * Name of the function
         */
        std::string functionName;

        /**
         * Runtime environment
         */
        std::string runtime;

        /**
         * Role
         */
        std::string role;

        /**
         * Version
         */
        std::string version;

        /**
         * Role
         */
        std::string handler;

        /**
         * Environment
         */
        EnvironmentVariables environment{};

        /**
         * Memory size in MB. Default: 128, Range: 128 - 10240 MB
         */
        long memorySize = 128;

        /**
         * Temporary disk space in MB
         */
        EphemeralStorage ephemeralStorage{};

        /**
         * Code
         */
        Code code{};

        /**
         * Tags
         */
        std::map<std::string, std::string> tags{};

        /**
         * Timeout
         */
        int timeout = 15 * 60;

        /**
         * @brief Parse a JSON string.
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_REQUEST_H
