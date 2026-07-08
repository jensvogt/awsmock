//
// Created by vogje01 on 07/08/2026
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Create usage plan key response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateUsagePlanKeyResponse final : Common::BaseCounter<CreateUsagePlanKeyResponse> {

        /**
         * API key ID
         */
        std::string id;

        /**
         * API key name
         */
        std::string name;

        /**
         * Key type (always "API_KEY")
         */
        std::string type{"API_KEY"};

        /**
         * Raw key value
         */
        std::string value;

      private:

        friend CreateUsagePlanKeyResponse tag_invoke(boost::json::value_to_tag<CreateUsagePlanKeyResponse>, boost::json::value const &v) {
            CreateUsagePlanKeyResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.type = Core::Json::GetStringValue(v, "type");
            r.value = Core::Json::GetStringValue(v, "value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUsagePlanKeyResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"id", obj.id},
                    {"name", obj.name},
                    {"type", obj.type},
                    {"value", obj.value},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
