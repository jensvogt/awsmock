//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_CREATE_APPLICATION_REQUEST_H
#define AWSMOCK_DTO_APPS_CREATE_APPLICATION_REQUEST_H

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

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
         * Application
         */
        Application application;

        /**
         * Application code as base64 encoded string
         */
        std::string code;

        /**
         * Application name prefix
         */
        std::string prefix;

        /**
         * Maximal number of results
         */
        long pageSize{};

        /**
         * Page index
         */
        long pageIndex{};

        /**
         * Sort columns
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend CreateApplicationRequest tag_invoke(boost::json::value_to_tag<CreateApplicationRequest>, boost::json::value const &v) {
            CreateApplicationRequest r;
            r.application = boost::json::value_to<Application>(v.at("application"));
            r.code = Core::Json::GetStringValue(v, "code");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateApplicationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"application", boost::json::value_from(obj.application)},
                    {"code", obj.code},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_CREATE_APPLICATION_REQUEST_H
