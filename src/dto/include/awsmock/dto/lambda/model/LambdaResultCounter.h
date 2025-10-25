//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_LAMBDA_RESULT_COUNTER_H
#define AWSMOCK_DTO_LAMBDA_RESULT_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/lambda/Instance.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief AWS Lambda result counter DTO.
     *
     * @par
     * Used only internally by the frontend.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LambdaResultCounter final : Common::BaseCounter<LambdaResultCounter> {

        /**
         * Database OID
         */
        std::string oid;

        /**
         * Lambda name
         */
        std::string lambdaName;

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Lambda runtime
         */
        std::string runtime;

        /**
         * Request body
         */
        std::string requestBody;

        /**
         * Response body
         */
        std::string responseBody;

        /**
         * Log messages
         */
        std::string logMessages;

        /**
         * Execution duration
         */
        long duration{};

        /**
         * Instance ID
         */
        std::string instanceId;

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * Status code
         */
        std::string httpStatusCode;

        /**
         * Lambda status
         */
        Database::Entity::Lambda::LambdaInstanceStatus lambdaStatus = Database::Entity::Lambda::InstanceUnknown;

        /**
         *
         * Last function StartServer
         */
        system_clock::time_point timestamp = system_clock::now();

      private:

        friend LambdaResultCounter tag_invoke(boost::json::value_to_tag<LambdaResultCounter>, boost::json::value const &v) {
            LambdaResultCounter r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.lambdaName = Core::Json::GetStringValue(v, "lambdaName");
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.runtime = Core::Json::GetStringValue(v, "runtime");
            r.requestBody = Core::Json::GetStringValue(v, "requestBody");
            r.responseBody = Core::Json::GetStringValue(v, "responseBody");
            r.logMessages = Core::Json::GetStringValue(v, "logMessages");
            r.duration = Core::Json::GetLongValue(v, "duration");
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.httpStatusCode = Core::Json::GetStringValue(v, "httpStatusCode");
            r.lambdaStatus = Database::Entity::Lambda::LambdaInstanceStatusFromString(Core::Json::GetStringValue(v, "lambdaStatus"));
            r.timestamp = Core::DateTimeUtils::FromISO8601(v.at("timestamp").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaResultCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"oid", obj.oid},
                    {"lambdaName", obj.lambdaName},
                    {"lambdaArn", obj.lambdaArn},
                    {"runtime", obj.runtime},
                    {"requestBody", obj.requestBody},
                    {"responseBody", obj.responseBody},
                    {"logMessages", obj.logMessages},
                    {"duration", obj.duration},
                    {"instanceId", obj.instanceId},
                    {"containerId", obj.containerId},
                    {"httpStatusCode", obj.httpStatusCode},
                    {"lambdaStatus", Database::Entity::Lambda::LambdaInstanceStatusToString(obj.lambdaStatus)},
                    {"timestamp", Core::DateTimeUtils::ToISO8601(obj.timestamp)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_RESULT_COUNTER_H
