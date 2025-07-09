//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_APPS_APPLICATION_H
#define AWSMOCK_DB_ENTITY_APPS_APPLICATION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::Apps {

    using std::chrono::system_clock;

    /**
     * @brief Application entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Application {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * Aws region
         */
        std::string region;

        /**
         * Application name
         */
        std::string name;

        /**
         * Application runtime
         */
        std::string runtime;

        /**
         * Application type
         */
        std::string type;

        /**
         * Application archive
         */
        std::string archive;

        /**
         * Application version
         */
        std::string version;

        /**
         * Application docker container ID
         */
        std::string containerId;

        /**
         * Application status
         */
        std::string status;

        /**
         * Enabled
         */
        bool enabled;

        /**
         * Environment
         */
        std::map<std::string, std::string> environment;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Options
         */
        std::map<std::string, std::string> options;

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
         * @param application application entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Application &application);
    };

}// namespace AwsMock::Database::Entity::Apps

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_H
