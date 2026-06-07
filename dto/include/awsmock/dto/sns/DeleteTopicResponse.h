//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct DeleteTopicResponse final : Common::BaseCounter<DeleteTopicResponse> {

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                root.add("DeleteTopicResponse.ResponseMetadata.RequestId", requestId);

                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "SNS"};

        friend DeleteTopicResponse tag_invoke(boost::json::value_to_tag<DeleteTopicResponse>, boost::json::value const &v) {
            DeleteTopicResponse r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteTopicResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
