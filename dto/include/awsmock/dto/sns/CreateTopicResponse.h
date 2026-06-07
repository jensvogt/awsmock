//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct CreateTopicResponse final : Common::BaseCounter<CreateTopicResponse> {

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
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                root.add("CreateTopicResponse.CreateTopicResult.TopicArn", topicArn);
                root.add("CreateTopicResponse.CreateTopicResult.Name", topicName);
                root.add("CreateTopicResponse.CreateTopicResult.Owner", owner);
                root.add("CreateTopicResponse.ResponseMetadata.RequestId", requestId);
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "SNS"};

        friend CreateTopicResponse tag_invoke(boost::json::value_to_tag<CreateTopicResponse>, boost::json::value const &v) {
            CreateTopicResponse r;
            r.topicName = Core::Json::GetStringValue(v, "topicName");
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.owner = Core::Json::GetStringValue(v, "owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateTopicResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicName", obj.topicName},
                    {"topicArn", obj.topicArn},
                    {"owner", obj.owner},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
