//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_MESSAGE_RESPONSE_H
#define AWSMOCK_DTO_SQS_DELETE_MESSAGE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct DeleteMessageResponse final : Common::BaseCounter<DeleteMessageResponse> {

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            // Root
            boost::property_tree::ptree pt;
            pt.put("DeleteMessageResponse.ResponseMetadata.RequestId", requestId);
            pt.put("DeleteMessageResponse.ResponseMetadata.Resource", "sqs");
            return Core::XmlUtils::ToXmlString(pt);
        }

      private:

        friend DeleteMessageResponse tag_invoke(boost::json::value_to_tag<DeleteMessageResponse>, boost::json::value const &v) {
            DeleteMessageResponse r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_MESSAGE_RESPONSE_H
