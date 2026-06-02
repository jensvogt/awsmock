//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_IS_DLQ_RESPONSE_H
#define AWSMOCK_DTO_SQS_IS_DLQ_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Is DLQ response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct IsDlqResponse final : Common::BaseCounter<IsDlqResponse> {

        /**
         * Main queue
         */
        std::vector<std::string> mainQueues;

        /**
         * DLQ flag
         */
        bool isDlq{};

      private:

        friend IsDlqResponse tag_invoke(boost::json::value_to_tag<IsDlqResponse>, boost::json::value const &v) {
            IsDlqResponse r;
            r.isDlq = Core::Json::GetBoolValue(v, "isDlq");
            if (Core::Json::AttributeExists(v, "mainQueues")) {
                for (const auto &mainQueue: v.at("mainQueues").as_array()) {
                    r.mainQueues.emplace_back(mainQueue.as_string());
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, IsDlqResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"mainQueues", boost::json::value_from(obj.mainQueues)},
                    {"isDlq", obj.isDlq},
            };
        }
    };

}// namespace Awsmock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_IS_DLQ_RESPONSE_H
