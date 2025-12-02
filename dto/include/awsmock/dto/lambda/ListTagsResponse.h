//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_TAGS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_TAGS_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct ListTagsResponse final : Common::BaseCounter<ListTagsResponse> {

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend ListTagsResponse tag_invoke(boost::json::value_to_tag<ListTagsResponse>, boost::json::value const &v) {
            ListTagsResponse r;
            r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTagsResponse const &obj) {
            jv = {
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_TAGS_REQUEST_H
