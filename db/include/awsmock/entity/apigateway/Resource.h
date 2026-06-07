//
// Created by vogje01 on 6/7/26.
//

#pragma once

// C++ includes
#include <map>

// Awsmock includes
#include <awsmock/entity/apigateway/ResourceMethod.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::ApiGateway {

    /**
     * @brief API gateway rest entity
     *
     * @par
     * The rest entity defines a rest endpoint inside the API gateway.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Resource final : Common::BaseEntity<Resource> {

        /**
         * @brief Resource ID
         */
        std::string id;

        /**
         * @brief Parent ID
         */
        std::string parentId;

        /**
         * @brief Resource path
         */
        std::string path;

        /**
         * @brief Path part
         */
        std::string pathPart;

        /**
         * @brief Resource methods
         */
        std::map<std::string, ResourceMethod> resourceMethods;

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
