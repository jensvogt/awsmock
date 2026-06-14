//
// Created by vogje01 on 6/13/26.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief API gateway authorizer DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Authorizer final : Common::BaseObject<Authorizer> {

        /**
         * @brief Authorizer ID
         */
        std::string id;

        /**
         * @brief Authorizer name
         */
        std::string name;

        /**
         * @brief Authorizer type (REQUEST, TOKEN, COGNITO_USER_POOLS)
         */
        std::string type;

        /**
         * @brief Auth type (custom, aws_iam, cognito_user_pools)
         */
        std::string authType;

        /**
         * @brief URI of the authorizer Lambda function
         */
        std::string authorizerUri;

        /**
         * @brief Identity source expression
         */
        std::string identitySource;

        /**
         * @brief TTL in seconds for cached authorizer results
         */
        long authorizerResultTtlInSeconds{};

      private:

        friend Authorizer tag_invoke(boost::json::value_to_tag<Authorizer>, boost::json::value const &v) {
            Authorizer a;
            a.id = Core::Json::GetStringValue(v, "id");
            a.name = Core::Json::GetStringValue(v, "name");
            a.type = Core::Json::GetStringValue(v, "type");
            a.authType = Core::Json::GetStringValue(v, "authType");
            a.authorizerUri = Core::Json::GetStringValue(v, "authorizerUri");
            a.identitySource = Core::Json::GetStringValue(v, "identitySource");
            a.authorizerResultTtlInSeconds = Core::Json::GetLongValue(v, "authorizerResultTtlInSeconds");
            return a;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Authorizer const &obj) {
            jv = {
                    {"id", obj.id},
                    {"name", obj.name},
                    {"type", obj.type},
                    {"authType", obj.authType},
                    {"authorizerUri", obj.authorizerUri},
                    {"identitySource", obj.identitySource},
                    {"authorizerResultTtlInSeconds", obj.authorizerResultTtlInSeconds},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
