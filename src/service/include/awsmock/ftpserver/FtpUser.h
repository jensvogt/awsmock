#pragma once

#include <awsmock/ftpserver/Permissions.h>
#include <string>
#include <utility>

namespace AwsMock::FtpServer {

    struct FtpUser {
        FtpUser(std::string username, std::string password, std::string local_root_path, const Permission permissions)
            : _username(std::move(username)), password_(std::move(password)), local_root_path_(std::move(local_root_path)), permissions_(permissions) {}

        const std::string _username;
        const std::string password_;
        const std::string local_root_path_;
        const Permission permissions_;
    };
}// namespace AwsMock::FtpServer