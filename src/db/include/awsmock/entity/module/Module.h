//
// Created by vogje01 on 10/22/23.
//

#ifndef AWSMOCK_DB_ENTITY_MODULE_MODULE_H
#define AWSMOCK_DB_ENTITY_MODULE_MODULE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::Module {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    /**
     * @brief AwsMock module entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Module {

        /**
         * ID
         */
        std::string oid;

        /**
         * Name
         */
        std::string name;

        /**
         * State
         */
        ModuleState state;

        /**
         * Status
         */
        ModuleStatus status;

        /**
         * HTTP port
         */
        int port = -1;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();
        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);

        /**
         * Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @param os output stream
         * @param module module entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Module &module);
    };

    typedef std::vector<Module> ModuleList;

}// namespace AwsMock::Database::Entity::Module

#endif// AWSMOCK_DB_ENTITY_MODULE_MODULE_H
