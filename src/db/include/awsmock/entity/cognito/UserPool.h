//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_H

// C++ includes
#include <string>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>


// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/cognito/UserPoolClient.h>
#include <awsmock/entity/cognito/UserPoolDomain.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::Cognito {

    using bsoncxx::to_json;
    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    /**
     * @brief Cognito user pool entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPool {

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the entity to a string representation.
         *
         * @return entity as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param userPool userPool entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const UserPool &userPool);
    };

    typedef std::vector<Entity::Cognito::UserPool> UserPoolList;

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_H
