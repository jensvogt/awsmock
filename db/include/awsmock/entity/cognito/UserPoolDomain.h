//
// Created by vogje01 on 5/27/24.
//

#pragma once

// C++ includes
#include <string>

namespace Awsmock::Database::Entity::Cognito {

    /**
     * @brief Cognito user pool domain
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPoolDomain {

        /**
         * Domain name
         */
        std::string domain;
    };

}// namespace Awsmock::Database::Entity::Cognito
