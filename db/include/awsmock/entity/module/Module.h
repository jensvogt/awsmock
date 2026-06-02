//
// Created by vogje01 on 10/22/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/utils/MongoUtils.h>

namespace Awsmock::Database::Entity::Module {

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
         * @brief Logger channel
         */
        std::string logChannel;

        /**
         * @brief Logger log level
         */
        std::string logLevel;

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
    };

}// namespace Awsmock::Database::Entity::Module
