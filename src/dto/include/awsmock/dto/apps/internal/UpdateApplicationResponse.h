//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_UPDATE_APPLICATION_RESPONSE_H
#define AWSMOCK_DTO_APPS_UPDATE_APPLICATION_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
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
     *      "environment": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *      }],
     *      "tags": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *      }],
     *      "options": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteApplicationResponse final : Common::BaseCounter<DeleteApplicationResponse> {

        /**
         * Application
         */
        Application application;

      private:

        friend DeleteApplicationResponse tag_invoke(boost::json::value_to_tag<DeleteApplicationResponse>, boost::json::value const &v) {
            DeleteApplicationResponse r;
            r.application = boost::json::value_to<Application>(v, "application");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteApplicationResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"application", boost::json::value_from(obj.application)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_CREATE_APPLICATION_RESPONSE_H
