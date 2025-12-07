//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/lambda/model/Environment.h>
#include <awsmock/dto/lambda/model/EphemeralStorage.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create function response
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionName": "ftp-file-copy",
     *   "FunctionArn": "arn:aws:lambda:eu-central-1:000000000000:function:ftp-file-copy",
     *   "Runtime": "java11",
     *   "Role": "arn:aws:iam::000000000000:role/ignoreme",
     *   "Handler": "org.springframework.cloud.function.adapter.aws.FunctionInvoker",
     *   "CodeSize": 23155331,
     *   "Description": "",
     *   "Timeout": 3,
     *   "MemorySize": 128,
     *   "LastModified": "2023-06-06T12:52:34.721631+0000",
     *   "CodeSha256": "kkUavVRsVEFvEIXyUwhl+n8/oaz50GYVleY9F3MiuYU=",
     *   "Version": "$LATEST",
     *     "Environment": {
     *       "Variables": {
     *         "JAVA_TOOL_OPTIONS": "-Duser.timezone=Europe/Berlin -Dspring.profiles.active=localstack"
     *       }
     *     },
     *   "TracingConfig": {
     *      "Mode": "PassThrough"
     *    },
     *   "RevisionId": "24e41752-636b-4f0f-840d-92bcc4e572ff",
     *   "State": "Pending",
     *   "StateReason": "The function is being created.",
     *   "StateReasonCode": "Creating",
     *   "PackageType": "Zip",
     *   "Architectures": [
     *     "x86_64"
     *   ],
     *   "EphemeralStorage": {
     *     "Size": 512
     *   },
     *   "SnapStart": {
     *     "ApplyOn": "None",
     *     "OptimizationStatus": "Off"
     *    },
     *    "RuntimeVersionConfig": {
     *    "RuntimeVersionArn": "arn:aws:lambda:eu-central-1::runtime:8eeff65f6809a3ce81507fe733fe09b835899b99481ba22fd75b5a7338290ec1"
     *   }
     * }
     * @endcode
     */
    struct CreateFunctionResponse final : Common::BaseCounter<CreateFunctionResponse> {

        /**
         * Name of the function
         */
        std::string functionArn;

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
         * Memory size
         */
        long memorySize = 128;

        /**
         * Code size
         */
        long codeSize;

        /**
         * Description
         */
        std::string description;

        /**
         * Timeout
         */
        int timeout = 3;

        /**
         * Code SHA256
         */
        std::string codeSha256;

        /**
         * Temporary disk space in MB
         */
        EphemeralStorage ephemeralStorage;

        /**
         * Modified
         */
        system_clock::time_point modified;

        /**
         * Docker image ID
         */
        std::string dockerImageId;

        /**
         * Docker container ID
         */
        std::string dockerContainerId;

      private:

        friend CreateFunctionResponse tag_invoke(boost::json::value_to_tag<CreateFunctionResponse>, boost::json::value const &v) {
            CreateFunctionResponse r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.functionName = Core::Json::GetStringValue(v, "FunctionName");
            r.runtime = Core::Json::GetStringValue(v, "Runtime");
            r.role = Core::Json::GetStringValue(v, "Role");
            r.handler = Core::Json::GetStringValue(v, "Handler");
            r.memorySize = Core::Json::GetLongValue(v, "MemorySize");
            r.codeSize = Core::Json::GetLongValue(v, "CodeSize");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.timeout = Core::Json::GetIntValue(v, "Timeout");
            r.codeSha256 = Core::Json::GetStringValue(v, "CodeSha256");
            r.dockerImageId = Core::Json::GetStringValue(v, "DockerImageId");
            r.dockerContainerId = Core::Json::GetStringValue(v, "DockerContainerId");
            r.modified = Core::Json::GetDatetimeValue(v, "LastModified");
            if (Core::Json::AttributeExists(v, "Environment")) {
                r.environment = boost::json::value_to<EnvironmentVariables>(v.at("Environment"));
            }
            if (Core::Json::AttributeExists(v, "EphemeralStorage")) {
                r.ephemeralStorage = boost::json::value_to<EphemeralStorage>(v.at("EphemeralStorage"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateFunctionResponse const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"FunctionName", obj.functionName},
                    {"Runtime", obj.runtime},
                    {"Role", obj.role},
                    {"Handler", obj.handler},
                    {"MemorySize", obj.memorySize},
                    {"CodeSize", obj.codeSize},
                    {"Description", obj.description},
                    {"Timeout", obj.timeout},
                    {"CodeSha256", obj.codeSha256},
                    {"DockerImageId", obj.dockerImageId},
                    {"DockerContainerId", obj.dockerContainerId},
                    {"LastModified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"Environment", boost::json::value_from(obj.environment)},
                    {"EphemeralStorage", boost::json::value_from(obj.ephemeralStorage)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_RESPONSE_H
