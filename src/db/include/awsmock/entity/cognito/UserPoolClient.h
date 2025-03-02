//
// Created by vogje01 on 5/31/24.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_CLIENT_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_CLIENT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
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
     * @brief Cognito user pool client entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPoolClient {

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
        bool generateSecret;

        /**
         * Access token validity
         */
        long accessTokenValidity;

        /**
         * ID token validity
         */
        long idTokenValidity;

        /**
         * Refresh token validity
         */
        long refreshTokenValidity;

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(std::optional<view> mResult);

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
         * @param userPoolClient userPool client entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const UserPoolClient &userPoolClient);
    };

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_CLIENT_H
