//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_TAG_RESOURCE_RESPONSE_H
#define AWSMOCK_DTO_SNS_TAG_RESOURCE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct TagResourceResponse final : Common::BaseCounter<TagResourceResponse> {

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] static std::string ToXml() {

            try {

                boost::property_tree::ptree root;
                root.add("TagResourceResponse.TagResourceResult", "");
                root.add("TagResourceResponse.ResponseMetadata.RequestId", Core::StringUtils::CreateRandomUuid());
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend TagResourceResponse tag_invoke(boost::json::value_to_tag<TagResourceResponse>, boost::json::value const &v) {
            TagResourceResponse r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagResourceResponse const &obj) {
            jv = {
                    {"requestId", obj.requestId},
            };
        }
    };

    typedef std::map<std::string, std::string> TagList;

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_TAG_RESOURCE_RESPONSE_H
