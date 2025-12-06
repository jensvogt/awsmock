//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_GET_APPLICATION_RESPONSE_H
#define AWSMOCK_DTO_APPS_GET_APPLICATION_RESPONSE_H

// AwsMock includes
#include "awsmock/dto/apps/model/Application.h"


#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Get application response
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
    struct GetApplicationResponse final : Common::BaseCounter<GetApplicationResponse> {

        /**
         * Application entities
         */
        Application application;

      private:

        friend GetApplicationResponse tag_invoke(boost::json::value_to_tag<GetApplicationResponse>, boost::json::value const &v) {
            GetApplicationResponse r;
            if (Core::Json::AttributeExists(v, "application")) {
                r.application = boost::json::value_to<Application>(v.at("application"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetApplicationResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"application", boost::json::value_from(obj.application)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_GET_APPLICATION_RESPONSE_H
