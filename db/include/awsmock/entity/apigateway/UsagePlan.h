//
// Created by vogje01 on 07/08/2026
//

#pragma once

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::ApiGateway {

    /**
     * @brief API gateway usage plan quota settings
     */
    struct UsagePlanQuota {

        long limit{};

        long offset{};

        std::string period;

        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        void FromDocument(const std::optional<view> &doc);
    };

    /**
     * @brief API gateway usage plan throttle settings
     */
    struct UsagePlanThrottle {

        long burstLimit{};

        double rateLimit{};

        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        void FromDocument(const std::optional<view> &doc);
    };

    /**
     * @brief API gateway usage plan entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UsagePlan final : Common::BaseEntity<UsagePlan> {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * AWS region
         */
        std::string region;

        /**
         * Usage plan ID
         */
        std::string id;

        /**
         * Usage plan name
         */
        std::string name;

        /**
         * Description
         */
        std::string description;

        /**
         * Quota settings
         */
        UsagePlanQuota quota;

        /**
         * Throttle settings
         */
        UsagePlanThrottle throttle;

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
         * @return entity as MongoDB document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace Awsmock::Database::Entity::ApiGateway
