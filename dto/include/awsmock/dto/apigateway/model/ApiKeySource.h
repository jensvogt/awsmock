//
// Created by vogje01 on 9/5/25.
//

#pragma once

// C++ includes
#include <algorithm>
#include <map>

namespace Awsmock::Dto::ApiGateway {

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
        const auto it = std::ranges::find_if(ApiKeySourceTypeNames, [&](const auto &p) { return p.second == ApiKeySourceType; });
        return it != ApiKeySourceTypeNames.end() ? it->first : ApiKeySourceType::UNKNOWN;
    }

}// namespace Awsmock::Dto::ApiGateway
