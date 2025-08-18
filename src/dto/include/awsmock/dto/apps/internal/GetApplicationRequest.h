//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_GET_APPLICATION_REQUEST_H
#define AWSMOCK_DTO_APPS_GET_APPLICATION_REQUEST_H

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Get an application
     *
     * Example:
     * @code{.json}
     * {
     *   "name": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetApplicationRequest final : Common::BaseCounter<GetApplicationRequest> {

        /**
         * Application name
         */
        std::string name;

      private:

        friend GetApplicationRequest tag_invoke(boost::json::value_to_tag<GetApplicationRequest>, boost::json::value const &v) {
            GetApplicationRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetApplicationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_GET_APPLICATION_REQUEST_H
