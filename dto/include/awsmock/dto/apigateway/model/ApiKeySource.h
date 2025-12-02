//
// Created by vogje01 on 9/5/25.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_API_KEY_SOURCE_H
#define AWSMOCK_DTO_API_GATEWAY_API_KEY_SOURCE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Rest API source type
     *
     * @par
     * The source of the API key for metering requests according to a usage plan. Valid values are: HEADER to read the API key
     * from the X-API-Key header of a request. AUTHORIZER to read the API key from the UsageIdentifierKey from a custom authorizer.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ApiKeySourceType {
        HEADER,
        AUTHORIZER,
        UNKNOWN
    };

    static std::map<ApiKeySourceType, std::string> ApiKeySourceTypeNames{
            {ApiKeySourceType::HEADER, "HEADER"},
            {ApiKeySourceType::AUTHORIZER, "AUTHORIZER"},
            {ApiKeySourceType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string ApiKeySourceTypeToString(const ApiKeySourceType &ApiKeySourceType) {
        return ApiKeySourceTypeNames[ApiKeySourceType];
    }

    [[maybe_unused]] static ApiKeySourceType ApiKeySourceTypeFromString(const std::string &ApiKeySourceType) {
        for (auto &[fst, snd]: ApiKeySourceTypeNames) {
            if (snd == ApiKeySourceType) {
                return fst;
            }
        }
        return ApiKeySourceType::UNKNOWN;
    }

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_API_KEY_SOURCE_H
