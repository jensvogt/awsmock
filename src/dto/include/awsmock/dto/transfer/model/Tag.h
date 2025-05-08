//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_TRANSFER_TAG_H
#define AWSMOCK_DTO_TRANSFER_TAG_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct Tag final : Common::BaseCounter<Tag> {

        /**
         * Key
         */
        std::string key;

        /**
         * Value
         */
        std::string value;

      private:

        friend Tag tag_invoke(boost::json::value_to_tag<Tag>, boost::json::value const &v) {
            Tag r;
            r.key = Core::Json::GetStringValue(v, "Key");
            r.value = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Tag const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", obj.key},
                    {"Value", obj.value},
            };
        }
    };

    typedef std::vector<Tag> TagList;

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_TAG_H
