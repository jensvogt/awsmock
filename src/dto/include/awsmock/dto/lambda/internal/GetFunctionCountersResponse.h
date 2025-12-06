//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_GET_FUNCTION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_GET_FUNCTION_COUNTERS_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Get function counters response
     *
     * Example:
     * @code{.json}
     * {
     *   "Code": {
     *      "ImageUri": "string",
     *      "Location": "string",
     *      "RepositoryType": "string",
     *      "ResolvedImageUri": "string"
     *   },
     *   "Concurrency": {
     *      "ReservedConcurrentExecutions": number
     *   },
     *   "Configuration": {
     *      "Architectures": [ "string" ],
     *      "CodeSha256": "string",
     *      "CodeSize": number,
     *      "DeadLetterConfig": {
     *         "TargetArn": "string"
     *      },
     *      "Description": "string",
     *      "Environment": {
     *         "Error": {
     *            "ErrorCode": "string",
     *            "Message": "string"
     *         },
     *         "Variables": {
     *            "string" : "string"
     *         }
     *      },
     *      "EphemeralStorage": {
     *         "Size": number
     *      },
     *      "FileSystemConfigs": [
     *         {
     *            "Arn": "string",
     *            "LocalMountPath": "string"
     *         }
     *      ],
     *      "FunctionArn": "string",
     *      "FunctionName": "string",
     *      "Handler": "string",
     *      "ImageConfigResponse": {
     *         "Error": {
     *            "ErrorCode": "string",
     *            "Message": "string"
     *         },
     *         "ImageConfig": {
     *            "Command": [ "string" ],
     *            "EntryPoint": [ "string" ],
     *            "WorkingDirectory": "string"
     *         }
     *      },
     *      "KMSKeyArn": "string",
     *      "LastModified": "string",
     *      "LastUpdateStatus": "string",
     *      "LastUpdateStatusReason": "string",
     *      "LastUpdateStatusReasonCode": "string",
     *      "Layers": [
     *         {
     *            "Arn": "string",
     *            "CodeSize": number,
     *            "SigningJobArn": "string",
     *            "SigningProfileVersionArn": "string"
     *         }
     *      ],
     *      "LoggingConfig": {
     *         "ApplicationLogLevel": "string",
     *         "LogFormat": "string",
     *         "LogGroup": "string",
     *         "SystemLogLevel": "string"
     *      },
     *      "MasterArn": "string",
     *      "MemorySize": number,
     *      "PackageType": "string",
     *      "RevisionId": "string",
     *      "Role": "string",
     *      "Runtime": "string",
     *      "RuntimeVersionConfig": {
     *         "Error": {
     *            "ErrorCode": "string",
     *            "Message": "string"
     *         },
     *         "RuntimeVersionArn": "string"
     *      },
     *      "SigningJobArn": "string",
     *      "SigningProfileVersionArn": "string",
     *      "SnapStart": {
     *         "ApplyOn": "string",
     *         "OptimizationStatus": "string"
     *      },
     *      "State": "string",
     *      "StateReason": "string",
     *      "StateReasonCode": "string",
     *      "Timeout": number,
     *      "TracingConfig": {
     *         "Mode": "string"
     *      },
     *      "Version": "string",
     *      "VpcConfig": {
     *         "Ipv6AllowedForDualStack": boolean,
     *         "SecurityGroupIds": [ "string" ],
     *         "SubnetIds": [ "string" ],
     *         "VpcId": "string"
     *      }
     *   },
     *   "Tags": {
     *      "string" : "string"
     *   }
     *}
     * @endcode
     */
    struct GetFunctionCountersResponse final : Common::BaseCounter<GetFunctionCountersResponse> {

        /**
         * OID
         */
        std::string id;

        /**
         * AWS ARN
         */
        std::string functionArn;

        /**
         * Function name
         */
        std::string functionName;

        /**
         * Role
         */
        std::string role;

        /**
         * Handler
         */
        std::string handler;

        /**
         * Runtime
         */
        std::string runtime;

        /**
         * Version
         */
        std::string version;

        /**
         * Name of the base64 ZIP file
         */
        std::string zipFile;

        /**
         * Name of the S3 bucket
         */
        std::string s3Bucket;

        /**
         * Name of the S3 key
         */
        std::string s3Key;

        /**
         * Version of the S3 object
         */
        std::string s3ObjectVersion;

        /**
         * Size
         */
        long size{};

        /**
         * Concurrency
         */
        long concurrency{};

        /**
         * Instances
         */
        long instances{};

        /**
         * Invocation
         */
        long invocations{};

        /**
         * Invocation
         */
        long averageRuntime{};

        /**
         * Enabled
         */
        bool enabled;

        /**
         * Status
         */
        std::string state;

        /**
         * Environment
         */
        std::map<std::string, std::string> environment;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Last invocation
         */
        system_clock::time_point lastInvocation;

        /**
         * Last started
         */
        system_clock::time_point lastStarted;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

      private:

        friend GetFunctionCountersResponse tag_invoke(boost::json::value_to_tag<GetFunctionCountersResponse>, boost::json::value const &v) {
            GetFunctionCountersResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.functionName = Core::Json::GetStringValue(v, "functionName");
            r.role = Core::Json::GetStringValue(v, "role");
            r.handler = Core::Json::GetStringValue(v, "handler");
            r.runtime = Core::Json::GetStringValue(v, "runtime");
            r.version = Core::Json::GetStringValue(v, "version");
            r.zipFile = Core::Json::GetStringValue(v, "zipFile");
            r.s3Bucket = Core::Json::GetStringValue(v, "s3Bucket");
            r.s3Key = Core::Json::GetStringValue(v, "s3Key");
            r.s3ObjectVersion = Core::Json::GetStringValue(v, "s3ObjectVersion");
            r.size = Core::Json::GetLongValue(v, "size");
            r.concurrency = Core::Json::GetLongValue(v, "concurrency");
            r.instances = Core::Json::GetLongValue(v, "instances");
            r.invocations = Core::Json::GetLongValue(v, "invocations");
            r.averageRuntime = Core::Json::GetLongValue(v, "averageRuntime");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.state = Core::Json::GetStringValue(v, "state");
            r.environment = boost::json::value_to<std::map<std::string, std::string>>(v.at("environment"));
            r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            r.lastInvocation = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "lastInvocation"));
            r.lastStarted = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "lastStarted"));
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "modified"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetFunctionCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"id", obj.id},
                    {"functionArn", obj.functionArn},
                    {"functionName", obj.functionName},
                    {"role", obj.role},
                    {"handler", obj.handler},
                    {"runtime", obj.runtime},
                    {"version", obj.version},
                    {"zipFile", obj.zipFile},
                    {"s3Bucket", obj.s3Bucket},
                    {"s3Key", obj.s3Key},
                    {"s3ObjectVersion", obj.s3ObjectVersion},
                    {"size", obj.size},
                    {"concurrency", obj.concurrency},
                    {"instances", obj.instances},
                    {"invocations", obj.invocations},
                    {"averageRuntime", obj.averageRuntime},
                    {"enabled", obj.enabled},
                    {"state", obj.state},
                    {"environment", boost::json::value_from(obj.environment)},
                    {"tags", boost::json::value_from(obj.tags)},
                    {"lastInvocation", Core::DateTimeUtils::ToISO8601(obj.lastInvocation)},
                    {"lastStarted", Core::DateTimeUtils::ToISO8601(obj.lastStarted)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_GET_FUNCTION_COUNTERS_RESPONSE_H
