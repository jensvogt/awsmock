//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_REBUILD_APPLICATION_CODE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_REBUILD_APPLICATION_CODE_REQUEST_H

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Rebuild application from scratch
     *
     * Example:
     * @code{.json}
     * {
     *   "application": {
     *     ...
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RebuildApplicationCodeRequest final : Common::BaseCounter<RebuildApplicationCodeRequest> {

        /**
         * Application
         */
        Application application;

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

        friend RebuildApplicationCodeRequest tag_invoke(boost::json::value_to_tag<RebuildApplicationCodeRequest>, boost::json::value const &v) {
            RebuildApplicationCodeRequest r;
            r.application = boost::json::value_to<Application>(v.at("application"));
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RebuildApplicationCodeRequest const &obj) {
            jv = {
                    {"application", boost::json::value_from(obj.application)},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_LAMBDA_REBUILD_APPLICATION_CODE_REQUEST_H
