//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_TOPICS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_TOPICS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/sns/Topic.h>

namespace AwsMock::Dto::SNS {

    struct ListTopicsResponse final : Common::BaseCounter<ListTopicsResponse> {

        /**
         * List of topics
         */
        std::vector<std::string> topics;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Total number of topics
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

                if (!topics.empty()) {
                    boost::property_tree::ptree xmlTopics;
                    for (auto &it: topics) {
                        boost::property_tree::ptree xmlTopic;
                        xmlTopic.add("TopicArn", it);
                        xmlTopics.add_child("member", xmlTopic);
                    }
                    root.add_child("ListTopicsResponse.ListTopicsResult.Topics", xmlTopics);
                }
                root.add("ListTopicsResponse.ListTopicsResult.NextToken", nextToken);
                root.add("ListTopicsResponse.ResponseMetadata.RequestId", Core::StringUtils::CreateRandomUuid());

                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ListTopicsResponse tag_invoke(boost::json::value_to_tag<ListTopicsResponse>, boost::json::value const &v) {
            ListTopicsResponse r;
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.total = Core::Json::GetLongValue(v, "Total");
            if (Core::Json::AttributeExists(v, "Topics")) {
                r.topics = boost::json::value_to<std::vector<std::string>>(v.at("Topics"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTopicsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"nextToken", obj.nextToken},
                    {"total", obj.total},
                    {"topics", boost::json::value_from(obj.topics)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SQS_LIST_TOPICS_RESPONSE_H
