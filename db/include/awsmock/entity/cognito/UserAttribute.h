//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_ATTRIBUTE_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_ATTRIBUTE_H

// C++ includes
#include <string>
#include <vector>

namespace AwsMock::Database::Entity::Cognito {

    /**
     * @brief Cognito user attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserAttribute {

        /**
         * Name
         */
        std::string name;

        /**
         * Value
         */
        std::string value;
    };

    typedef std::vector<UserAttribute> UserAttributeList;

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_ATTRIBUTE_H
