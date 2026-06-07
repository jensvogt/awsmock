//
// Created by vogje01 on 6/7/26.
//

#pragma once

// Awsmock includes
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::ApiGateway {

    /**
     * @brief API gateway resource method
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ResourceMethod final : Common::BaseEntity<ResourceMethod> {

        /**
         * @brief Is an API key required flag
         */
        bool apiKeyRequired;

        /**
         * @brief HTTP method
         */
        std::string httpMethod;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as a MongoDB document.
         */
        [[nodiscard]]
        view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace Awsmock::Database::Entity::ApiGateway
