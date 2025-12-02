//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/cognito/UserPoolClient.h>
#include <awsmock/entity/cognito/UserPoolDomain.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::Cognito {

    /**
     * @brief Cognito user pool entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPool final : Common::BaseEntity<UserPool> {

        /**
         * MongoDB POD
         */
        std::string oid;

        /**
         * AWS region
         */
        std::string region;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Name
         */
        std::string name;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Key for a SHA256 KMS key
         */
        std::string kmsKey;

        /**
         * Domain
         */
        UserPoolDomain domain;

        /**
         * Clients
         */
        std::vector<UserPoolClient> userPoolClients;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Returns a client by clientId
         *
         * @param clientId user pool client ID
         * @return UserPoolClient
         */
        UserPoolClient GetClient(const std::string &clientId);

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

    typedef std::vector<UserPool> UserPoolList;

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_H
