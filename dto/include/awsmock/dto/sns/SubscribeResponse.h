//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct SubscribeResponse final : Common::BaseCounter<SubscribeResponse> {

        /**
         * @brief Subscription ARN
         */
        std::string subscriptionArn;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                root.add("SubscribeResponse.SubscribeResult.SubscriptionArn", subscriptionArn);
                root.add("SubscribeResponse.ResponseMetadata.RequestId", Core::StringUtils::CreateRandomUuid());
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        };

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "SNS"};

        friend SubscribeResponse tag_invoke(boost::json::value_to_tag<SubscribeResponse>, boost::json::value const &v) {
            SubscribeResponse r;
            r.subscriptionArn = Core::Json::GetStringValue(v, "subscriptionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SubscribeResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"subscriptionArn", obj.subscriptionArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
