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
    enum class ApiStatusType {
        UPDATING,
        AVAILABLE,
        PENDING,
        FAILED,
        UNKNOWN
    };

    static std::map<ApiStatusType, std::string> ApiStatusTypeNames{
            {ApiStatusType::UPDATING, "UPDATING"},
            {ApiStatusType::AVAILABLE, "AVAILABLE"},
            {ApiStatusType::PENDING, "PENDING"},
            {ApiStatusType::FAILED, "FAILED"},
            {ApiStatusType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]]
    static std::string ApiStatusToString(const ApiStatusType &apiStatusType) {
        return ApiStatusTypeNames[apiStatusType];
    }

    [[maybe_unused]]
    static ApiStatusType ApiSourceFromString(const std::string &s) {
        const auto it = std::ranges::find_if(ApiStatusTypeNames, [&](const auto &p) { return p.second == s; });
        return it != ApiStatusTypeNames.end() ? it->first : ApiStatusType::UNKNOWN;
    }

}// namespace Awsmock::Database::Entity::ApiGateway
