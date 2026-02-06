//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_LIST_APPLICATION_NAMES_RESPONSE_H
#define AWSMOCK_DTO_APPS_LIST_APPLICATION_NAMES_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief List application names response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListApplicationNamesResponse final : Common::BaseCounter<ListApplicationNamesResponse> {

        /**
         * Application entities
         */
        std::vector<std::string> applicationNames;

        /**
         * Total number of application entities
         */
        long total{};

      private:

        friend ListApplicationNamesResponse tag_invoke(boost::json::value_to_tag<ListApplicationNamesResponse>, boost::json::value const &v) {
            ListApplicationNamesResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "applicationNames")) {
                r.applicationNames = boost::json::value_to<std::vector<std::string>>(v.at("applicationNames"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListApplicationNamesResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"applicationNames", boost::json::value_from(obj.applicationNames)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_LIST_APPLICATION_COUNTERS_RESPONSE_H
