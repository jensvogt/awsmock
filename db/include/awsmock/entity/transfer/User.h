//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_USER_H
#define AWSMOCK_DB_ENTITY_USER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Database::Entity::Transfer {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;

    /**
     * @brief Transfer manager user entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct User {

        /**
         * User name
         */
        std::string userName;

        /**
         * Password
         */
        std::string password;

        /**
         * Home directory
         */
        std::string homeDirectory;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Role
         */
        std::string role;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(view mResult);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param m user
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const User &m);
    };

}// namespace AwsMock::Database::Entity::Transfer

#endif// AWSMOCK_DB_ENTITY_USER_H
