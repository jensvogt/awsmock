//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_API_GATEWAY_REST_API_H
#define AWSMOCK_DB_ENTITY_API_GATEWAY_REST_API_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::ApiGateway {

    /**
     * @brief API gateway rest entity
     *
     * @par
     * The rest entity defines a rest endpoint inside the API gateway.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RestApi final : Common::BaseEntity<RestApi> {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * Aws region
         */
        std::string region;

        /**
         * ID
         */
        std::string id;

        /**
         * Application name
         */
        std::string name;

        /**
         * Description
         */
        std::string description;

        /**
         * Endpoint URL
         */
        std::string endpointUrl;

        /**
         * API key
         */
        std::string apiKeySource;

        /**
         * Clone source
         */
        std::string cloneFrom;

        /**
         * Version
         */
        std::string version;

        /**
         * JSON policy
         */
        std::string policy;

        /**
         * Root resource ID
         */
        std::string rootResourceId;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Binary media types array
         */
        std::vector<std::string> binaryMediaTypes;

        /**
         * Warnings
         */
        std::vector<std::string> warnings;

        /**
         * Disable API endpoint
         */
        bool disableExecuteApiEndpoint;

        /**
         * Minimal compression size
         */
        long minimumCompressionSize;

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

}// namespace AwsMock::Database::Entity::ApiGateway

#endif// AWSMOCK_DB_ENTITY_API_GATEWAY_REST_API_H
