//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_FUNCTION_COUNTER_H
#define AWSMOCK_DTO_LAMBDA_FUNCTION_COUNTER_H

// C++ standard includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/DeadLetterConfig.h>
#include <awsmock/dto/lambda/model/Environment.h>

namespace AwsMock::Dto::Lambda {

    using std::chrono::system_clock;

    struct FunctionCounter final : Common::BaseCounter<FunctionCounter> {
        /**
         * Function ARN
         */
        std::string functionArn;

        /**
         * Function name
         */
        std::string functionName;

        /**
         * Description
         */
        std::string description;

        /**
         * SHA256 checksum
         */
        std::string codeSha256;

        /**
         * Function handler
         */
        std::string handler;

        /**
         * Name of the base64 ZIP file
         */
        std::string zipFile;

        /**
         * Function runtime
         */
        std::string runtime;

        /**
         * Version
         */
        std::string version;

        /**
         * Code size
         */
        long codeSize{};

        /**
         * Dead letter config
         */
        DeadLetterConfig deadLetterConfig;

        /**
         * Architectures
         */
        std::vector<std::string> architectures;

        /**
         * Last update state
         */
        std::string lastUpdateStatus;

        /**
         * Last update state reason
         */
        std::string lastUpdateStatusReason;

        /**
         * Last update state reason
         */
        std::string lastUpdateStatusReasonCode;

        /**
         * Enabled flag
         */
        bool enabled = false;

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
         * Timeout in minutes
         */
        long timeout = 15;

        /**
         * Environment
         */
        EnvironmentVariables environment;

        /**
         * Invocations
         */
        long invocations{};

        /**
         * Average runtime in milliseconds
         */
        long averageRuntime{};

        /**
         * Instances
         */
        long instances{};

        /**
         * Last modification datetime
         */
        system_clock::time_point lastModified;

      private:

        friend FunctionCounter tag_invoke(boost::json::value_to_tag<FunctionCounter>, boost::json::value const &v) {
            FunctionCounter r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.functionName = Core::Json::GetStringValue(v, "functionName");
            r.description = Core::Json::GetStringValue(v, "description");
            r.codeSha256 = Core::Json::GetStringValue(v, "codeSha256");
            r.handler = Core::Json::GetStringValue(v, "handler");
            r.runtime = Core::Json::GetStringValue(v, "runtime");
            r.zipFile = Core::Json::GetStringValue(v, "zipFile");
            r.version = Core::Json::GetStringValue(v, "version");
            r.codeSize = Core::Json::GetLongValue(v, "codeSize");
            r.deadLetterConfig = boost::json::value_to<DeadLetterConfig>(v.at("deadLetterConfig"));
            r.architectures = boost::json::value_to<std::vector<std::string>>(v.at("architectures"));
            r.lastUpdateStatus = Core::Json::GetStringValue(v, "lastUpdateStatus");
            r.lastUpdateStatusReason = Core::Json::GetStringValue(v, "lastUpdateStatusReason");
            r.lastUpdateStatusReasonCode = Core::Json::GetStringValue(v, "lastUpdateStatusReasonCode");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.state = Core::Json::GetStringValue(v, "state");
            r.stateReason = Core::Json::GetStringValue(v, "stateReason");
            r.stateReasonCode = Core::Json::GetStringValue(v, "stateReasonCode");
            r.timeout = Core::Json::GetLongValue(v, "timeout");
            r.environment = boost::json::value_to<EnvironmentVariables>(v.at("environment"));
            r.instances = Core::Json::GetLongValue(v, "instances");
            r.invocations = Core::Json::GetLongValue(v, "invocations");
            r.averageRuntime = Core::Json::GetLongValue(v, "averageRuntime");
            r.lastModified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "lastModified"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, FunctionCounter const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
                    {"functionName", obj.functionName},
                    {"description", obj.description},
                    {"codeSha256", obj.codeSha256},
                    {"handler", obj.handler},
                    {"runtime", obj.runtime},
                    {"zipFile", obj.zipFile},
                    {"version", obj.version},
                    {"codeSize", obj.codeSize},
                    {"deadLetterConfig", boost::json::value_from(obj.deadLetterConfig)},
                    {"architectures", boost::json::value_from(obj.architectures)},
                    {"lastUpdateStatus", obj.lastUpdateStatus},
                    {"lastUpdateStatusReason", obj.lastUpdateStatusReason},
                    {"lastUpdateStatusReasonCode", obj.lastUpdateStatusReasonCode},
                    {"enabled", obj.enabled},
                    {"state", obj.state},
                    {"stateReason", obj.stateReason},
                    {"stateReasonCode", obj.stateReasonCode},
                    {"timeout", obj.timeout},
                    {"environment", boost::json::value_from(obj.environment)},
                    {"instances", obj.instances},
                    {"invocations", obj.invocations},
                    {"averageRuntime", obj.averageRuntime},
                    {"lastModified", obj.averageRuntime},
            };
        }
    };

    typedef std::vector<FunctionCounter> FunctionCounterList;

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_FUNCTION_COUNTER_H
