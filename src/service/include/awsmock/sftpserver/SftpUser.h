//
// Created by vogje01 on 3/30/25.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_SERVER_SFTP_USER_H
#define AWSMOCK_SERVICE_TRANSFER_SERVER_SFTP_USER_H

// C++ includes
#include <string>
#include <vector>

namespace AwsMock::Service {

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

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_SERVER_SFTP_USER_H
