//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DTO_COGNITO_AUTH_FLOW_H
#define AWSMOCK_DTO_COGNITO_AUTH_FLOW_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Cognito auth flow enum
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class AuthFlowType {
        USER_SRP_AUTH,
        REFRESH_TOKEN_AUTH,
        REFRESH_TOKEN,
        CUSTOM_AUTH,
        ADMIN_NO_SRP_AUTH,
        USER_PASSWORD_AUTH,
        ADMIN_USER_PASSWORD_AUTH
    };

    static std::map<AuthFlowType, std::string> AuthFlowTypeNames{
            {AuthFlowType::USER_SRP_AUTH, "USER_SRP_AUTH"},
            {AuthFlowType::REFRESH_TOKEN_AUTH, "REFRESH_TOKEN_AUTH"},
            {AuthFlowType::REFRESH_TOKEN, "REFRESH_TOKEN"},
            {AuthFlowType::CUSTOM_AUTH, "CUSTOM_AUTH"},
            {AuthFlowType::ADMIN_NO_SRP_AUTH, "ADMIN_NO_SRP_AUTH"},
            {AuthFlowType::USER_PASSWORD_AUTH, "USER_PASSWORD_AUTH"},
            {AuthFlowType::ADMIN_USER_PASSWORD_AUTH, "ADMIN_USER_PASSWORD_AUTH"},
    };

    [[maybe_unused]] static std::string AuthFlowTypeToString(const AuthFlowType &authFlowType) {
        return AuthFlowTypeNames[authFlowType];
    }

    [[maybe_unused]] static AuthFlowType AuthFlowTypeFromString(const std::string &authFlowType) {
        for (auto &[fst, snd]: AuthFlowTypeNames) {
            if (snd == authFlowType) {
                return fst;
            }
        }
        return AuthFlowType::USER_PASSWORD_AUTH;
    }

}// namespace AwsMock::Dto::Cognito

#endif//AWSMOCK_DTO_COGNITO_AUTH_FLOW_H
