//
// Created by vogje01 on 6/13/26.
//

#pragma once

// AwsMock includes
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::ApiGateway {

    /**
     * @brief API gateway authorizer entity
     *
     * @code{json}
     * {
     *   "id": "sbd3op",
     *   "name": "image-publishing-authorizer",
     *   "type": "REQUEST",
     *   "authType": "custom",
     *   "authorizerUri": "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/arn:aws:lambda:eu-central-1:012096702153:function:pim-prod-image-publishing-infrastructure-authorizer/invocations",
     *   "identitySource": "method.request.querystring.apiKey, context.resourcePath",
     *   "authorizerResultTtlInSeconds": 3600
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Authorizer final : Common::BaseEntity<Authorizer> {

        /**
         * @brief Authorizer ID
         */
        std::string id{};

        /**
         * @brief Authorizer name
         */
        std::string name{};

        /**
         * @brief Authorizer type (REQUEST, TOKEN, COGNITO_USER_POOLS)
         */
        std::string type{};

        /**
         * @brief Auth type (custom, aws_iam, cognito_user_pools)
         */
        std::string authType{};

        /**
         * @brief URI of the authorizer Lambda function
         */
        std::string authorizerUri{};

        /**
         * @brief Identity source expression
         */
        std::string identitySource{};

        /**
         * @brief TTL in seconds for cached authorizer results
         */
        std::int64_t authorizerResultTtlInSeconds{};

        /**
         * @brief Creation timestamp
         */
        system_clock::time_point created = system_clock::now();

        /**
         * @brief Modification timestamp
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as a MongoDB document.
         */
        [[nodiscard]]
        view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace Awsmock::Database::Entity::ApiGateway
