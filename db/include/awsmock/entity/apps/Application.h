//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_APPS_APPLICATION_H
#define AWSMOCK_DB_ENTITY_APPS_APPLICATION_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::Apps {

    /**
     * @brief Application entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Application final : Common::BaseEntity<Application> {

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
         * Private port. This is the internal port used by the application inside the docker image.
         */
        long privatePort{};

        /**
         * Public port. This is the host port used by the application inside the docker image.
         */
        long publicPort{};

        /**
         * Name of the container
         */
        std::string containerName;

        /**
         * ID of the container
         */
        std::string containerId;

        /**
         * Application archive
         */
        std::string archive;

        /**
         * Application version
         */
        std::string version;

        /**
         * Application status
         */
        std::string status;

        /**
         * Enabled
         */
        bool enabled = false;

        /**
         * Application description
         */
        std::string description;

        /**
         * Environment
         */
        std::map<std::string, std::string> environment;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Application dependencies
         */
        std::vector<std::string> dependencies;

        /**
         * Docker image ID
         */
        std::string imageId;

        /**
         * Docker image size
         */
        long imageSize{};

        /**
         * Full image name in format name:tag
         */
        std::string imageName{};

        /**
         * MD5 of docker image
         */
        std::string imageMd5;

        /**
         * Last start time
         */
        system_clock::time_point lastStarted;

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

}// namespace AwsMock::Database::Entity::Apps

#endif// AWSMOCK_DB_ENTITY_APPS_APPLICATION_H
