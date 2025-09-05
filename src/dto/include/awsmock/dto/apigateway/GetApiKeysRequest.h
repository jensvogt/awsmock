//
// Created by vogje01 on 01/09/2025
//

#ifndef AWSMOCK_DTO_API_GATEWAY_GET_API_KEYS_REQUEST_H
#define AWSMOCK_DTO_API_GATEWAY_GET_API_KEYS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Delete API gateway key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetApiKeysRequest final : Common::BaseCounter<GetApiKeysRequest> {

        /**
         * Name query
         */
        std::string nameQuery;

        /**
         * ID of the customer
         */
        std::string customerId;

        /**
         * Include values
         */
        bool includeValues{};

        /**
         * ID of the last key in the list
         */
        std::string position;

        /**
         * Maximal number of items
         */
        long limit{};

      private:

        friend GetApiKeysRequest tag_invoke(boost::json::value_to_tag<GetApiKeysRequest>, boost::json::value const &v) {
            GetApiKeysRequest r;
            r.nameQuery = Core::Json::GetStringValue(v, "nameQuery");
            r.customerId = Core::Json::GetStringValue(v, "customerId");
            r.includeValues = Core::Json::GetBoolValue(v, "includeValues");
            r.position = Core::Json::GetStringValue(v, "position");
            r.limit = Core::Json::GetLongValue(v, "limit");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetApiKeysRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"nameQuery", obj.nameQuery},
                    {"customerId", obj.customerId},
                    {"includeValues", obj.includeValues},
                    {"position", obj.position},
                    {"limit", obj.limit},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_GET_API_KEYS_REQUEST_H
