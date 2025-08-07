//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_DOMAIN_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_DOMAIN_H

namespace AwsMock::Database::Entity::Cognito {

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

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_POOL_DOMAIN_H
