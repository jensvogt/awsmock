//
// Created by vogje01 on 06/09/2023.
//

#pragma once

// C++ standard includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/DeadLetterConfig.h>
#include <awsmock/dto/lambda/model/Environment.h>

namespace Awsmock::Dto::Lambda {

    struct Function final : Common::BaseObject<Function> {

        /**
         * Architectures
         */
        std::vector<std::string> architectures = {};

        /**
         * SHA256 checksum
         */
        std::string codeSha256 = {};

        /**
         * Code size
         */
        long codeSize = 0;

        /**
         * Dead letter config
         */
        DeadLetterConfig deadLetterConfig = {};

        /**
         * Description
         */
        std::string description = {};
        // TODO: Environment, EpheremalStorage, FileSystemConfig

        /**
         * Function ARN
         */
        std::string functionArn = {};

        /**
         * Function name
         */
        std::string functionName = {};

        /**
         * Function handler
         */
        std::string handler = {};

        /**
         * Function runtime
         */
        std::string runtime = {};

        // TODO: ImageConfigResponse

        /**
         * Last modification datetime
         */
        system_clock::time_point lastModified = {};

        /**
         * Last update state
         */
        std::string lastUpdateStatus = {};

        /**
         * Last update state reason
         */
        std::string lastUpdateStatusReason = {};

        /**
         * Last update state reason
         */
        std::string lastUpdateStatusReasonCode = {};

        /**
         * Minimal concurrency
         */
        int minConcurrency{};

        /**
         * Maximal concurrency
         */
        int maxConcurrency{};

        /**
         * State
         */
        std::string state = {};

        /**
         * State reason
         */
        std::string stateReason = {};

        /**
         * State reason code
         */
        std::string stateReasonCode = {};

        /**
         * Timeout
         */
        long timeout = 15 * 60;

        /**
         * Version
         */
        std::string version = "latest";

        /**
         * Enabled
         */
        bool enabled = false;

        /**
         * Environment
         */
        EnvironmentVariables environment = {};

        /**
         * Tags
         */
        std::map<std::string, std::string> tags = {};

      private:

        friend Function tag_invoke(boost::json::value_to_tag<Function>, boost::json::value const &v) {
            Function r;
            r.codeSha256 = Core::Json::GetStringValue(v, "CodeSha256");
            r.codeSize = Core::Json::GetLongValue(v, "CodeSize");
            r.deadLetterConfig = boost::json::value_to<DeadLetterConfig>(v.at("DeadLetterConfig"));
            r.description = Core::Json::GetStringValue(v, "Description");
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.functionName = Core::Json::GetStringValue(v, "FunctionName");
            r.handler = Core::Json::GetStringValue(v, "Handler");
            r.runtime = Core::Json::GetStringValue(v, "Runtime");
            r.lastUpdateStatus = Core::Json::GetStringValue(v, "LastUpdateStatus");
            r.lastUpdateStatusReason = Core::Json::GetStringValue(v, "LastUpdateStatusReason");
            r.lastUpdateStatusReasonCode = Core::Json::GetStringValue(v, "LastUpdateStatusReasonCode");
            r.state = Core::Json::GetStringValue(v, "State");
            r.minConcurrency = Core::Json::GetIntValue(v, "MinConcurrency");
            r.maxConcurrency = Core::Json::GetIntValue(v, "Concurrency");
            r.stateReason = Core::Json::GetStringValue(v, "StateReason");
            r.stateReasonCode = Core::Json::GetStringValue(v, "StateReasonCode");
            r.timeout = Core::Json::GetIntValue(v, "Timeout");
            r.version = Core::Json::GetStringValue(v, "Version");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            if (Core::Json::AttributeExists(v, "Architectures")) {
                r.architectures = boost::json::value_to<std::vector<std::string>>(v.at("Architectures"));
            }
            if (Core::Json::AttributeExists(v, "Environment")) {
                r.environment = boost::json::value_to<EnvironmentVariables>(v.at("Environment"));
            }
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Function const &obj) {
            jv = {
                    {"CodeSha256", obj.codeSha256},
                    {"CodeSize", obj.codeSize},
                    {"DeadLetterConfig", boost::json::value_from(obj.deadLetterConfig)},
                    {"Description", obj.description},
                    {"FunctionArn", obj.functionArn},
                    {"FunctionName", obj.functionName},
                    {"Handler", obj.handler},
                    {"Runtime", obj.runtime},
                    {"LastUpdateStatus", obj.lastUpdateStatus},
                    {"LastUpdateStatusReason", obj.lastUpdateStatusReason},
                    {"LastUpdateStatusReasonCode", obj.lastUpdateStatusReasonCode},
                    {"State", obj.state},
                    {"StateReason", obj.stateReason},
                    {"StateReasonCode", obj.stateReasonCode},
                    {"Timeout", obj.timeout},
                    {"Version", obj.version},
                    {"Enabled", obj.enabled},
                    {"Architectures", boost::json::value_from(obj.architectures)},
                    {"Environment", boost::json::value_from(obj.environment)},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
