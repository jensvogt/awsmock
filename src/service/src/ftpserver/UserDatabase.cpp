#include "awsmock/ftpserver/UserDatabase.h"

#include <iostream>

namespace AwsMock::FtpServer {

    bool UserDatabase::addUser(const std::string &username, const std::string &password, const std::string &local_root_path, Permission permissions) {
        const std::lock_guard database_lock(database_mutex_);

        if (isUsernameAnonymousUser(username)) {
            if (anonymous_user_) {
                log_error << "Error adding user with username \"" << username
                          << "\". The username denotes the anonymous user, which is already present.";
                return false;
            }
            anonymous_user_ = std::make_shared<FtpUser>(username, password, local_root_path, permissions);
            log_debug << "Successfully added anonymous user.";
            return true;
        }
        if (const auto user_it = database_.find(username); user_it == database_.end()) {
            database_.emplace(username, std::make_shared<FtpUser>(username, password, local_root_path, permissions));
            log_debug << "Successfully added user: " << username << " home: " << local_root_path;
            return true;
        }
        std::cerr << "Error adding user with username \"" << username << "\". The user already exists.";
        return false;
    }

    std::shared_ptr<FtpUser> UserDatabase::getUser(const std::string &username, const std::string &password) const {
        const std::lock_guard database_lock(database_mutex_);

        if (isUsernameAnonymousUser(username)) {
            return anonymous_user_;
        }
        if (const auto user_it = database_.find(username); user_it == database_.end()) {
            return nullptr;
        } else {
            if (user_it->second->password_ == password)
                return user_it->second;
            else
                return nullptr;
        }
    }

    bool UserDatabase::isUsernameAnonymousUser(const std::string &username) const// NOLINT(readability-convert-member-functions-to-static) Reason: I don't want to break the API. Otherwise this is a good finding and should be accepted.
    {
        return (username.empty() || username == "ftp" || username == "anonymous");
    }

}// namespace AwsMock::FtpServer
