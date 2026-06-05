//
// Created by vogje01 on 11/25/23.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

namespace Awsmock::Database::Entity::Cognito {

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

}// namespace Awsmock::Database::Entity::Cognito
