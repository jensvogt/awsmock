//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    using std::chrono::system_clock;

    struct CreateDeploymentResponse final : Common::BaseCounter<CreateDeploymentResponse> {

        std::string id;
        std::string description;
        system_clock::time_point createdDate;

      private:

        friend CreateDeploymentResponse tag_invoke(boost::json::value_to_tag<CreateDeploymentResponse>, boost::json::value const &v) {
            CreateDeploymentResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.description = Core::Json::GetStringValue(v, "description");
            r.createdDate = Core::Json::GetDatetimeValue(v, "createdDate");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateDeploymentResponse const &obj) {
            jv = {
                    {"id", obj.id},
                    {"description", obj.description},
                    {"createdDate", Core::DateTimeUtils::ToISO8601(obj.createdDate)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
