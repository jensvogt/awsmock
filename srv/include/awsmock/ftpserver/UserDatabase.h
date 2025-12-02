#pragma once

// C++ includes
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <iostream>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/ftpserver/FtpUser.h>
#include <awsmock/repository/CognitoDatabase.h>

namespace AwsMock::FtpServer {

    class UserDatabase {

    public:

        /**
         * Constructor
         */
        UserDatabase();

        /**
         * Add a new user to the database.
         *
         * // TODO:Should be replaced by Aws User database
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
        static void CreateDirectories(const std::string &userName);

        /**
         * Server mutex
         */
        mutable std::mutex database_mutex_;

        /**
         * User database
         */
        std::map<std::string, std::shared_ptr<FtpUser> > database_;

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
}// namespace AwsMock::FtpServer