//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LAMBDA_TRIGGER_COUNTER_H
#define AWSMOCK_DTO_SQS_LAMBDA_TRIGGER_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct LambdaTriggerCounter final : Common::BaseCounter<LambdaTriggerCounter> {
        /**
         * UUID
         */
        std::string uuid;

        /**
         * AWS resource ARN
         */
        std::string arn;

        /**
         * Enabled flag
         */
        bool enabled = true;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend LambdaTriggerCounter tag_invoke(boost::json::value_to_tag<LambdaTriggerCounter>, boost::json::value const &v) {
            LambdaTriggerCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.uuid = v.at("uuid").as_string();
            r.arn = v.at("arn").as_string();
            r.enabled = v.at("enabled").as_bool();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaTriggerCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"uuid", obj.uuid},
                    {"arn", obj.arn},
                    {"enabled", obj.enabled},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LAMBDA_TRIGGER_COUNTER_H
