//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_GET_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_GET_FUNCTION_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include "model/Function.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/Code.h>
#include <awsmock/dto/lambda/model/EphemeralStorage.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Get function response
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
    struct GetFunctionResponse final : Common::BaseCounter<GetFunctionResponse> {

        /**
         * Configuration
         */
        Function configuration;

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
         * State
         */
        std::string state;

        /**
         * State reason
         */
        std::string stateReason;

        /**
         * State reason code
         */
        std::string stateReasonCode;

        /**
         * Last update status
         */
        std::string lastUpdateStatus = "Successful";

      private:

        friend GetFunctionResponse tag_invoke(boost::json::value_to_tag<GetFunctionResponse>, boost::json::value const &v) {
            GetFunctionResponse r;
            r.timeout = Core::Json::GetIntValue(v, "Timeout");
            r.state = Core::Json::GetStringValue(v, "State");
            r.stateReason = Core::Json::GetStringValue(v, "StateReason");
            r.stateReasonCode = Core::Json::GetStringValue(v, "StateReasonCode");
            r.lastUpdateStatus = Core::Json::GetStringValue(v, "LastUpdateStatus");
            if (Core::Json::AttributeExists(v, "Function")) {
                r.configuration = boost::json::value_to<Function>(v.at("Function"));
            }
            if (Core::Json::AttributeExists(v, "EphemeralStorage")) {
                r.ephemeralStorage = boost::json::value_to<EphemeralStorage>(v.at("EphemeralStorage"));
            }
            if (Core::Json::AttributeExists(v, "Code")) {
                r.code = boost::json::value_to<Code>(v.at("Code"));
            }
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetFunctionResponse const &obj) {
            jv = {
                    {"Timeout", obj.timeout},
                    {"State", obj.state},
                    {"StateReason", obj.stateReason},
                    {"StateReasonCode", obj.stateReasonCode},
                    {"LastUpdateStatus", obj.lastUpdateStatus},
                    {"Tags", boost::json::value_from(obj.tags)},
                    {"Function", boost::json::value_from(obj.configuration)},
                    {"EphemeralStorage", boost::json::value_from(obj.ephemeralStorage)},
                    {"Code", boost::json::value_from(obj.code)},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_GET_FUNCTION_REQUEST_H
