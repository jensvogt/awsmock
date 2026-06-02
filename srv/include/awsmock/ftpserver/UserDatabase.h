#pragma once

// C++ includes
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

// AwsMock includes
#include <../../../../db/include/awsmock/repository/cognito/CognitoMongoRepository.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/ftpserver/FtpUser.h>

namespace Awsmock::FtpServer {

    class UserDatabase {

      public:

        /**
         * Constructor
         */
        UserDatabase();

        /**
         * Add a new user to the database.
         *
         * @param username name of the user
         * @param password user password
         * @param local_root_path  local path
         * @param permissions permissions
         * @return
         */
        bool addUser(const std::string &username, const std::string &password, const std::string &local_root_path, Permission permissions);

        /**
         * Return a user by name
         *
         * @param username user name
         * @param password user password
         * @return FtpUser
         */
        std::shared_ptr<FtpUser> getUser(const std::string &username, const std::string &password) const;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Transfer"};

        /**
         * Check whether the user is the anonymous user
         *
         * @param username name of the user
         * @return
         */
        static bool isUsernameAnonymousUser(const std::string &username);

        /**
         * @brief Create default directories for the user
         *
         * @param userName name of the user
         */
        void CreateDirectories(const std::string &userName) const;

        /**
         * Server mutex
         */
        mutable std::mutex database_mutex_;

        /**
         * User database
         */
        std::map<std::string, std::shared_ptr<FtpUser>> database_;

        /**
         * Anonymous user
         */
        std::shared_ptr<FtpUser> anonymous_user_;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * Cognito user pool ID
         */
        std::string _userPoolId;

        /**
         * FTP server base dir
         */
        std::string _baseDir;
    };
}// namespace Awsmock::FtpServer
