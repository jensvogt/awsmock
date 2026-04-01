//
// Created by vogje01 on 5/31/24.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_CLIENT_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_CLIENT_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::Cognito {

    /**
     * @brief Cognito user pool client entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPoolClient final : Common::BaseEntity<UserPoolClient> {

        /**
         * UserPoolId
         */
        std::string userPoolId;

        /**
         * Client ID
         */
        std::string clientId;

        /**
         * Client name
         */
        std::string clientName;

        /**
         * Client secret. This is actually a KMS symmetric key ID. Used for encrypting/decrypting client passwords.
         */
        std::string clientSecret;

        /**
         * Generate secret flag
         */
        bool generateSecret{};

        /**
         * Access token validity
         */
        long accessTokenValidity{};

        /**
         * ID token validity
         */
        long idTokenValidity{};

        /**
         * Refresh token validity
         */
        long refreshTokenValidity{};

        /**
         * Auth flows
         */
        std::vector<std::string> explicitAuthFlows = {"ADMIN_NO_SRP_AUTH", "CUSTOM_AUTH_FLOW_ONLY", "USER_PASSWORD_AUTH", "ALLOW_ADMIN_USER_PASSWORD_AUTH", "ALLOW_CUSTOM_AUTH", "ALLOW_USER_PASSWORD_AUTH", "ALLOW_USER_SRP_AUTH", "ALLOW_REFRESH_TOKEN_AUTH", "ALLOW_USER_AUTH"};

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_CLIENT_H
