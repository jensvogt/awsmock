//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CREATE_TAG_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_CREATE_TAG_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct CreateTagRequest final : Common::BaseCounter<CreateTagRequest> {

        /**
         * ARN
         */
        std::string arn;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateTagRequest tag_invoke(boost::json::value_to_tag<CreateTagRequest>, boost::json::value const &v) {
            CreateTagRequest r;
            r.arn = Core::Json::GetStringValue(v, "Arn");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateTagRequest const &obj) {
            jv = {
                    {"Arn", obj.arn},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CREATE_TAG_REQUEST_H
