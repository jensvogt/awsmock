//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_CREATE_APPLICATION_RESPONSE_H
#define AWSMOCK_DTO_APPS_CREATE_APPLICATION_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Create application response
     *
     * Example:
     * @code{.json}
     * {
     *   "Application": {
     *      "name": number,
     *      "runtime": "string",
     *      "archive": "string",
     *      "enabled": boolean,
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateApplicationResponse final : Common::BaseCounter<CreateApplicationResponse> {

        /**
         * Applications
         */
        std::vector<Application> applications;

        /**
         * Total number of application entities
         */
        long total{};

      private:

        friend CreateApplicationResponse tag_invoke(boost::json::value_to_tag<CreateApplicationResponse>, boost::json::value const &v) {
            CreateApplicationResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "applications")) {
                r.applications = boost::json::value_to<std::vector<Application>>(v.at("applications"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateApplicationResponse const &obj) {
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

#endif// AWSMOCK_DTO_APPS_CREATE_APPLICATION_RESPONSE_H
