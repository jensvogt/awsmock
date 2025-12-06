//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_SUBSCRIPTIONS_BY_TOPIC_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_SUBSCRIPTIONS_BY_TOPIC_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/sns/model/Subscription.h>

namespace AwsMock::Dto::SNS {

    struct ListSubscriptionsByTopicResponse final : Common::BaseCounter<ListSubscriptionsByTopicResponse> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Subscriptions
         */
        std::vector<Subscription> subscriptions;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Total number of subscriptions
         */
        long total{};

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;

                if (!subscriptions.empty()) {
                    boost::property_tree::ptree xmlTopics;
                    for (auto &it: subscriptions) {
                        boost::property_tree::ptree xmlTopic;
                        xmlTopic.add("TopicArn", it.topicArn);
                        xmlTopic.add("SubscriptionArn", it.subscriptionArn);
                        xmlTopic.add("Protocol", it.protocol);
                        xmlTopic.add("Endpoint", it.endpoint);
                        xmlTopic.add("Owner", it.owner);
                        xmlTopics.add_child("member", xmlTopic);
                    }
                    root.add_child("ListSubscriptionsByTopicResponse.ListSubscriptionsByTopicResult.Subscriptions", xmlTopics);
                }
                root.add("ListSubscriptionsByTopicResponse.ResponseMetadata.RequestId", Core::StringUtils::CreateRandomUuid());

                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ListSubscriptionsByTopicResponse tag_invoke(boost::json::value_to_tag<ListSubscriptionsByTopicResponse>, boost::json::value const &v) {
            ListSubscriptionsByTopicResponse r;
            r.topicArn = Core::Json::GetStringValue(v, "TopicArn");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.total = Core::Json::GetLongValue(v, "Total");
            if (Core::Json::AttributeExists(v, "Subscriptions")) {
                r.subscriptions = boost::json::value_to<std::vector<Subscription>>(v.at("Subscriptions"));
            }

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSubscriptionsByTopicResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"subscriptions", boost::json::value_from(obj.subscriptions)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_SUBSCRIPTIONS_BY_TOPIC_RESPONSE_H
