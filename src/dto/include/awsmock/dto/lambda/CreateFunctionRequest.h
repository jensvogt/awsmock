//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_REQUEST_H

// C++ standard includes
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseRequest.h>
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
    struct CreateFunctionRequest : public Dto::Common::BaseRequest {

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
         * Role
         */
        std::string handler;

        /**
         * Environment
         */
        EnvironmentVariables environment;

        /**
         * Memory size in MB. Default: 128, Range: 128 - 10240 MB
         */
        long memorySize = 128;

        /**
         * Temporary disk space in MB
         */
        EphemeralStorage ephemeralStorage;

        /**
         * Code
         */
        Code code;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Timeout
         */
        int timeout = 3;

        /**
         * Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Parse a JSON stream.
         *
         * @code{.json}
         *"{\"JAVA_TOOL_OPTIONS\":\"-Duser.timezone=Europe/Berlin -Dspring.profiles.active=localhost\"}"
         * @endcode
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const CreateFunctionRequest &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_REQUEST_H
