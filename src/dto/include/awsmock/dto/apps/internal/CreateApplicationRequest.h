//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_CREATE_APPLICATION_REQUEST_H
#define AWSMOCK_DTO_APPS_CREATE_APPLICATION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Create an application
     *
     * @par
     * Request to create a new application.
     *
     * Example:
     * @code{.json}
     * {
     *   "application":
     *   {
     *     "name": "string",
     *     "runtime": "string",
     *     "archive": number,
     *     "enabled": "string",
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateApplicationRequest final : Common::BaseCounter<CreateApplicationRequest> {

        /**
         * UserPoolId
         */
        Application application;

      private:

        friend CreateApplicationRequest tag_invoke(boost::json::value_to_tag<CreateApplicationRequest>, boost::json::value const &v) {
            CreateApplicationRequest r;
            r.application = boost::json::value_to<Application>(v.at("userPoolId"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateApplicationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"application", boost::json::value_from(obj.application)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_CREATE_APPLICATION_REQUEST_H
