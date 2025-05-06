//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_GET_TOPIC_ATTRIBUTES_RESPONSE_H
#define AWSMOCK_DTO_SNS_GET_TOPIC_ATTRIBUTES_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct GetTopicAttributesResponse final : Common::BaseCounter<GetTopicAttributesResponse> {

        /**
         * Name
         */
        std::string topicName;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Display name
         */
        std::string displayName;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;

                boost::property_tree::ptree attributesArray;
                boost::property_tree::ptree attributesElement1;
                attributesElement1.put("key", "TopicArn");
                attributesElement1.put("value", topicArn);
                attributesArray.push_back(std::make_pair("entry", attributesElement1));

                boost::property_tree::ptree attributesElement2;
                attributesElement2.put("key", "Owner");
                attributesElement2.put("value", owner);
                attributesArray.push_back(std::make_pair("entry", attributesElement2));

                boost::property_tree::ptree attributesElement3;
                attributesElement3.put("key", "DisplayName");
                attributesElement3.put("value", displayName);
                attributesArray.push_back(std::make_pair("entry", attributesElement3));

                root.add_child("GetTopicAttributesResponse.GetTopicAttributesResult.Attributes", attributesArray);
                root.add("GetTopicAttributesResponse.ResponseMetadata.RequestId", requestId);

                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend GetTopicAttributesResponse tag_invoke(boost::json::value_to_tag<GetTopicAttributesResponse>, boost::json::value const &v) {
            GetTopicAttributesResponse r;
            r.topicName = Core::Json::GetStringValue(v, "TopicName");
            r.topicArn = Core::Json::GetStringValue(v, "TopicArn");
            r.owner = Core::Json::GetStringValue(v, "Owner");
            r.displayName = Core::Json::GetStringValue(v, "DisplayName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetTopicAttributesResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicName", obj.topicName},
                    {"topicArn", obj.topicArn},
                    {"owner", obj.owner},
                    {"displayName", obj.displayName},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_GET_TOPIC_ATTRIBUTES_RESPONSE_H
