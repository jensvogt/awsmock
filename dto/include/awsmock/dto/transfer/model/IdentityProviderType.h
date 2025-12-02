//
// Created by JVO on 29.04.2024.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_TYPE_H
#define AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_TYPE_H

// C++ standard includes
#include <map>
#include <string>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Transfer identity provider type
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class IdentityProviderType {
        SERVICE_MANAGED,
        API_GATEWAY,
        AWS_DIRECTORY_SERVICE,
        AWS_LAMBDA,
        UNKNOWN
    };

    static std::map<IdentityProviderType, std::string> IdentityProviderTypeNames{
            {IdentityProviderType::SERVICE_MANAGED, "ServiceManaged"},
            {IdentityProviderType::API_GATEWAY, "ApiGateway"},
            {IdentityProviderType::AWS_DIRECTORY_SERVICE, "AwsDirectory"},
            {IdentityProviderType::AWS_LAMBDA, "Lambda"},
            {IdentityProviderType::UNKNOWN, "UNKNOWN"},
    };

    static std::string IdentityProviderTypeToString(const IdentityProviderType identityProviderType) {
        return IdentityProviderTypeNames[identityProviderType];
    }

    static IdentityProviderType IdentityProviderTypeFromString(const std::string &identityProviderType) {
        for (auto &[fst, snd]: IdentityProviderTypeNames) {
            if (snd == identityProviderType) {
                return fst;
            }
        }
        return IdentityProviderType::UNKNOWN;
    }


}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_TYPE_H
