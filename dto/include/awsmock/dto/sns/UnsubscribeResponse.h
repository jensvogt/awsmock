//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_UNSUBSCRIBE_RESPONSE_H
#define AWSMOCK_DTO_SNS_UNSUBSCRIBE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct UnsubscribeResponse final : Common::BaseCounter<UnsubscribeResponse> {

        /**
         * @brief Subscription ARN
         */
        std::string subscriptionArn;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        std::string ToXml() const {
            try {

                boost::property_tree::ptree root;
                root.add("UnsubscribeResponse.SubscriptionArn", subscriptionArn);
                root.add("UnsubscribeResponse.ResponseMetadata.RequestId", requestId);
                return Core::XmlUtils::ToXmlString(root);

            } catch (const std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        };

      private:

        friend UnsubscribeResponse tag_invoke(boost::json::value_to_tag<UnsubscribeResponse>, boost::json::value const &v) {
            UnsubscribeResponse r;
            r.subscriptionArn = Core::Json::GetStringValue(v, "ResourceArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UnsubscribeResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"subscriptionArn", obj.subscriptionArn},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_UNSUBSCRIBE_RESPONSE_H
