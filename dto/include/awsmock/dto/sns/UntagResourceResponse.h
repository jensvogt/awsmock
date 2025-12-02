//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_UNTAG_RESOURCE_RESPONSE_H
#define AWSMOCK_DTO_SNS_UNTAG_RESOURCE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct UntagResourceResponse final : Common::BaseCounter<UntagResourceResponse> {

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        static std::string ToXml() {

            try {

                boost::property_tree::ptree root;
                root.add("UntagResourceResponse.UntagResourceResult", "");
                root.add("UntagResourceResponse.ResponseMetadata.RequestId", Core::StringUtils::CreateRandomUuid());
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend UntagResourceResponse tag_invoke(boost::json::value_to_tag<UntagResourceResponse>, boost::json::value const &v) {
            UntagResourceResponse r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UntagResourceResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_UNTAG_RESOURCE_RESPONSE_H
