//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_API_GATEWAY_REST_H
#define AWSMOCK_DB_ENTITY_API_GATEWAY_REST_H

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
    struct Rest final : Common::BaseEntity<Rest> {

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

#endif// AWSMOCK_DB_ENTITY_API_GATEWAY_KEY_H
