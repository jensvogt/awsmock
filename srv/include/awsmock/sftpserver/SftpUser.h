//
// Created by vogje01 on 3/30/25.
//

#pragma once
// C++ includes
#include <string>
#include <vector>

namespace Awsmock::Service {

    struct SftpUser {

        /**
         * User name
         */
        std::string userName;

        /**
         * Password
         */
        std::string password;

        /**
         * Home directory
         */
        std::string homeDirectory;
    };

    typedef std::vector<SftpUser> SftpUsers;

}// namespace Awsmock::Service

