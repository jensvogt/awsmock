//
// Created by vogje01 on 11/25/23.
//

#pragma once

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::ApiGateway {

    /**
     * @brief API gateway key entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ApiKey final : Common::BaseEntity<ApiKey> {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * API ID
         */
        std::string id;

        /**
         * Aws region
         */
        std::string region;

        /**
         * Application name
         */
        std::string name;

        /**
         * Customer ID
         */
        std::string customerId;

        /**
         * Description
         */
        std::string description;

        /**
         * Enabled
         */
        bool enabled{};

        /**
         * Generate distinct
         */
        bool generateDistinct{};

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Usage plan ID this key is associated with
         */
        std::string usagePlanId;

        /**
         * Value
         */
        std::string keyValue;

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
         * @return entity as a MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace Awsmock::Database::Entity::ApiGateway
