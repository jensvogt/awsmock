#include "awsmock/ftpserver/UserDatabase.h"

namespace AwsMock::FtpServer {

    UserDatabase::UserDatabase() {
        _region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        _userPoolId = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.user-pool-id");
    }

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
            log_info << "Anonymous user logged in";
            return anonymous_user_;
        }

        try {
            if (Database::Entity::Cognito::User user = Database::CognitoDatabase::instance().GetUserByUserName(_region, _userPoolId, username); user.password == password) {
                return std::make_shared<FtpUser>(user.userName, user.password, user.userName, FtpServer::Permission::All);
            }
            log_warning << "User not found, userName: " << username;
            return nullptr;
        } catch (Core::DatabaseException &exc) {
            log_warning << "User not found userName: " << username;
            return nullptr;
        }
    }

    bool UserDatabase::isUsernameAnonymousUser(const std::string &username) {
        return (username.empty() || username == "ftp" || username == "anonymous");
    }

}// namespace AwsMock::FtpServer