//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_DELETE_PARAMETER_COUNTER_REQUEST_H
#define AWSMOCK_DTO_SSM_DELETE_PARAMETER_COUNTER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief List all parameter request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteParameterCounterRequest final : Common::BaseCounter<DeleteParameterCounterRequest> {

        /**
         * Parameter name
         */
        std::string name;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        long pageSize = 10;

        /**
         * Page index
         */
        long pageIndex = 0;

        /**
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend DeleteParameterCounterRequest tag_invoke(boost::json::value_to_tag<DeleteParameterCounterRequest>, boost::json::value const &v) {
            DeleteParameterCounterRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteParameterCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_DELETE_PARAMETER_COUNTER_REQUEST_H
