//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_RESTART_ALL_APPLICATIONS_REQUEST_H
#define AWSMOCK_DTO_APPS_RESTART_ALL_APPLICATIONS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Restart all applications
     *
     * @par
     * Request to restart all  applications.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RestartAllApplicationsRequest final : Common::BaseCounter<RestartAllApplicationsRequest> {

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

        friend RestartAllApplicationsRequest tag_invoke(boost::json::value_to_tag<RestartAllApplicationsRequest>, boost::json::value const &v) {
            RestartAllApplicationsRequest r;
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RestartAllApplicationsRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_RESTART_ALL_APPLICATIONS_REQUEST_H
