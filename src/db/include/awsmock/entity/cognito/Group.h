//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_GROUP_H
#define AWSMOCK_DB_ENTITY_COGNITO_GROUP_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/cognito/UserAttribute.h>
#include <awsmock/entity/common/BaseEntity.h>

struct User;
namespace AwsMock::Database::Entity::Cognito {

    /**
     * @brief Cognito group entity
     *
     * This class contains the users in the group.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Group final : Common::BaseEntity<Group> {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * Aws region
         */
        std::string region;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Group name
         */
        std::string groupName;

        /**
         * Description
         */
        std::string description;

        /**
         * Role ARN
         */
        std::string roleArn;

        /**
         * Precedence
         */
        long precedence{};

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

    typedef std::vector<Group> GroupList;

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_GROUP_H
