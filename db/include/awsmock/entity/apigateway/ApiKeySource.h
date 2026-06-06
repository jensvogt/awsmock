//
// Created by vogje01 on 01/06/2023.
//

#pragma once

// C++ includes
#include <map>
#include <string>

namespace Awsmock::Database::Entity::ApiGateway {

    /**
     * @brief The source of the API key for metering requests according to a usage plan.
     *
     * @par
     * Valid values are: HEADER to read the API key from the
     * X-API-Key header of a request. AUTHORIZER to read the API key from the UsageIdentifierKey from a custom authorizer.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum ApiKeySourceType {
        HEADER,
        AUTHORIZER,
        UNKNOWN
    };

    static std::map<ApiKeySourceType, std::string> ApiKeySourceTypeNames{
            {HEADER, "HEADER"},
            {AUTHORIZER, "AUTHORIZER"},
            {UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]]
    static std::string ApiKeySourceTypeToString(const ApiKeySourceType &apiSourceType) {
        return ApiKeySourceTypeNames[apiSourceType];
    }

    [[maybe_unused]]
    static ApiKeySourceType ApiKeySourceTypeFromString(const std::string &s) {
        const auto it = std::ranges::find_if(ApiKeySourceTypeNames, [&](const auto &p) { return p.second == s; });
        return it != ApiKeySourceTypeNames.end() ? it->first : UNKNOWN;
    }

}// namespace Awsmock::Database::Entity::ApiGateway
