//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_PUBLISH_RESPONSE_H
#define AWSMOCK_DTO_SNS_PUBLISH_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct PublishResponse final : Common::BaseCounter<PublishResponse> {

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("PublishResponse.PublishResult.MessageId", messageId);
            root.add("PublishResponse.ResponseMetadata.RequestId", requestId);
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend PublishResponse tag_invoke(boost::json::value_to_tag<PublishResponse>, boost::json::value const &v) {
            PublishResponse r;
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PublishResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"messageId", obj.messageId},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_PUBLISH_RESPONSE_H
