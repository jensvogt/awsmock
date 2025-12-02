//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_LIST_APPLICATION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_APPS_LIST_APPLICATION_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief List application counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListApplicationCountersResponse final : Common::BaseCounter<ListApplicationCountersResponse> {

        /**
         * Application entities
         */
        std::vector<Application> applications;

        /**
         * Total number of application entities
         */
        long total{};

      private:

        friend ListApplicationCountersResponse tag_invoke(boost::json::value_to_tag<ListApplicationCountersResponse>, boost::json::value const &v) {
            ListApplicationCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "applications")) {
                r.applications = boost::json::value_to<std::vector<Application>>(v.at("applications"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListApplicationCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"applications", boost::json::value_from(obj.applications)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_LIST_APPLICATION_COUNTERS_RESPONSE_H
