//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_ENTITY_LAMBDA_EPHEMERAL_STORAGE_H
#define AWSMOCK_ENTITY_LAMBDA_EPHEMERAL_STORAGE_H

// C++ includes
#include <string>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>


// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::Lambda {

    using bsoncxx::to_json;
    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;

    /**
     * @brief Lambda ephemeral storage entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EphemeralStorage {

        /**
         * Temporary disk space in MB. Default: 512 MB, Range: 512 - 10240 MB
         */
        long size = 512;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult database document.
         */
        [[maybe_unused]] void FromDocument(std::optional<bsoncxx::document::view> mResult);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const EphemeralStorage &m);
    };

}// namespace AwsMock::Database::Entity::Lambda

#endif// AWSMOCK_ENTITY_LAMBDA_EPHEMERAL_STORAGE_H
