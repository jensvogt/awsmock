//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_GROUP_H
#define AWSMOCK_DB_ENTITY_COGNITO_GROUP_H

// C++ includes
#include <map>
#include <string>

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/JSON/Object.h>

// Boost includes
#include <boost/date_time/posix_time/posix_time.hpp>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/stdx.hpp>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/entity/cognito/UserAttribute.h>
#include <awsmock/entity/cognito/UserStatus.h>

struct User;
namespace AwsMock::Database::Entity::Cognito {

    using bsoncxx::to_json;
    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    /**
     * @brief Cognito group entity
     *
     * This class contains the users in the group.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Group {

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
        int precedence;

        /**
         * Users
         */
        //std::vector<User *&> users;

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
        void FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult);

        /**
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * @brief Converts the entity to a JSON object
         *
         * @param jsonObject JSON object.
         */
        void FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject);

        /**
         * @brief Converts the entity to a string representation.
         *
         * @return entity as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param group group entity
         * @return output string
         */
        friend std::ostream &operator<<(std::ostream &os, const Group &group);
    };

    typedef std::vector<Group> GroupList;

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_GROUP_H
