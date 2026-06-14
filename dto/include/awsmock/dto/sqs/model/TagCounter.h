//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::SQS {

    struct TagCounter final : Common::BaseObject<TagCounter> {

        /**
         * Key
         */
        std::string tagKey;

        /**
         * Value
         */
        std::string tagValue;

      private:

        friend TagCounter tag_invoke(boost::json::value_to_tag<TagCounter>, boost::json::value const &v) {
            TagCounter r;
            r.tagKey = v.at("tagKey").as_string();
            r.tagValue = v.at("tagValue").as_string();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagCounter const &obj) {
            jv = {
                    {"tagKey", obj.tagKey},
                    {"tagValue", obj.tagValue},
            };
        }
    };

}// namespace Awsmock::Dto::SQS
