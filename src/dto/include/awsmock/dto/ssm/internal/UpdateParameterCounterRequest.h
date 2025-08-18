//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_UPDATE_PARAMETER_COUNTER_REQUEST_H
#define AWSMOCK_DTO_SSM_UPDATE_PARAMETER_COUNTER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>
#include <awsmock/dto/ssm/model/ParameterType.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Update parameter request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateParameterCounterRequest final : Common::BaseCounter<UpdateParameterCounterRequest> {

        /**
         * Parameter name
         */
        std::string name;

        /**
         * Parameter value
         */
        std::string value;

        /**
         * Parameter description
         */
        std::string description;

        /**
         * Parameter type
         */
        ParameterType type;

        /**
         * KMS key ARN
         */
        std::string kmsKeyArn;

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

        friend UpdateParameterCounterRequest tag_invoke(boost::json::value_to_tag<UpdateParameterCounterRequest>, boost::json::value const &v) {
            UpdateParameterCounterRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.value = Core::Json::GetStringValue(v, "value");
            r.description = Core::Json::GetStringValue(v, "description");
            r.type = ParameterTypeFromString(Core::Json::GetStringValue(v, "type"));
            r.kmsKeyArn = Core::Json::GetStringValue(v, "kmsKeyArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateParameterCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"value", obj.value},
                    {"description", obj.description},
                    {"type", ParameterTypeToString(obj.type)},
                    {"kmsKeyArn", obj.kmsKeyArn},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_UPDATE_PARAMETER_COUNTER_REQUEST_H
