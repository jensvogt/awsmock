//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_TAG_COUNTER_H
#define AWSMOCK_DTO_SNS_TAG_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct TagCounter final : Common::BaseCounter<TagCounter> {

        /**
         * Key
         */
        std::string tagKey;

        /**
         * Value
         */
        std::string tagValue;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend TagCounter tag_invoke(boost::json::value_to_tag<TagCounter>, boost::json::value const &v) {
            TagCounter r;
            r.tagKey = Core::Json::GetStringValue(v, "tagKey");
            r.tagValue = Core::Json::GetStringValue(v, "tagValue");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagCounter const &obj) {
            jv = {
                    {"tagKey", obj.tagKey},
                    {"tagValue", obj.tagValue},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_TAG_COUNTER_H
