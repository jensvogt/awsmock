//
// Created by JVO on 29.04.2024.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_SFTP_AUTHENTICATION_METHOD_H
#define AWSMOCK_DTO_TRANSFER_SERVER_SFTP_AUTHENTICATION_METHOD_H

// C++ standard includes
#include <map>
#include <string>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief SFTP authentication method
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class SftpAuthenticationMethod {
        PASSWORD,
        PUBLIC_KEY,
        PUBLIC_KEY_OR_PASSWORD,
        PUBLIC_KEY_AND_PASSWORD,
        UNKNOWN
    };

    static std::map<SftpAuthenticationMethod, std::string> SftpAuthenticationMethodTypeNames{
            {SftpAuthenticationMethod::PASSWORD, "Password"},
            {SftpAuthenticationMethod::PUBLIC_KEY, "PublicKey"},
            {SftpAuthenticationMethod::PUBLIC_KEY_OR_PASSWORD, "PublicKeyOrPassword"},
            {SftpAuthenticationMethod::PUBLIC_KEY_AND_PASSWORD, "PublicKeyAndPassword"},
            {SftpAuthenticationMethod::UNKNOWN, "UNKNOWN"},
    };

    static std::string SftpAuthenticationMethodToString(const SftpAuthenticationMethod sftpAuthenticationMethod) {
        return SftpAuthenticationMethodTypeNames[sftpAuthenticationMethod];
    }

    static SftpAuthenticationMethod SftpAuthenticationMethodFromString(const std::string &sftpAuthenticationMethod) {
        for (auto &[fst, snd]: SftpAuthenticationMethodTypeNames) {
            if (snd == sftpAuthenticationMethod) {
                return fst;
            }
        }
        return SftpAuthenticationMethod::UNKNOWN;
    }

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_SERVER_SFTP_AUTHENTICATION_METHOD_H
