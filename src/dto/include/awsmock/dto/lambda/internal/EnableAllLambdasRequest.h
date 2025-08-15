//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_LAMBDA_ENABLE_ALL_APPLICATIONS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ENABLE_ALL_APPLICATIONS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Enable all lambdas
     *
     * @par
     * Request to enable all lambdas.
     *
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EnableAllLambdasRequest final : Common::BaseCounter<EnableAllLambdasRequest> {

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

        friend EnableAllLambdasRequest tag_invoke(boost::json::value_to_tag<EnableAllLambdasRequest>, boost::json::value const &v) {
            EnableAllLambdasRequest r;
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EnableAllLambdasRequest const &obj) {
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

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ENABLE_ALL_APPLICATIONS_REQUEST_H
