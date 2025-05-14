//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_TAGS_H
#define AWSMOCK_DB_ENTITY_LAMBDA_TAGS_H

// C++ includes
#include <sstream>
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>


// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::Lambda {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;

    /**
     * @brief Lambda tags entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Tags {

        /**
         * Variables
         */
        std::map<std::string, std::string> tags;

        /**
         * @brief Checks whether tags with the given tags key exist.
         *
         * @param key key of the tags
         * @return true if tags with the given key exist.
         */
        bool HasTag(const std::string &key);

        /**
         * @brief Returns a given tags value by key
         *
         * @param key name of the tag
         * @return found notification or notifications.end().
         */
        std::string GetTagValue(const std::string &key);

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);

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
         * @param os output stream
         * @param tag tag entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Tags &tag);
    };

}// namespace AwsMock::Database::Entity::Lambda

#endif// AWSMOCK_DB_ENTITY_LAMBDA_TAGS_H
