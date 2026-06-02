//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    using std::chrono::system_clock;

    /**
     * @brief Get API keys response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetApiKeysResponse final : Common::BaseCounter<GetApiKeysResponse> {

        /**
         * List of API keys as items
         */
        std::vector<Key> items;

        /**
         * ID of the last key in the list
         */
        std::string position;

        /**
         * Warnings array
         */
        std::vector<std::string> warnings;

      private:

        friend GetApiKeysResponse tag_invoke(boost::json::value_to_tag<GetApiKeysResponse>, boost::json::value const &v) {
            GetApiKeysResponse r;
            r.items = boost::json::value_to<std::vector<Key>>(v.at("item"));
            r.position = Core::Json::GetStringValue(v, "position");
            r.warnings = boost::json::value_to<std::vector<std::string>>(v.at("warnings"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetApiKeysResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"item", boost::json::value_from(obj.items)},
                    {"position", obj.position},
                    {"warnings", boost::json::value_from(obj.warnings)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
