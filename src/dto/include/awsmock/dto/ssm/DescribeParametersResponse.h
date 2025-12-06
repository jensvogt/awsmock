//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_DESCRIBE_PARAMETERS_RESPONSE_H
#define AWSMOCK_DTO_SSM_DESCRIBE_PARAMETERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/ssm/model/Parameter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Describe parameter response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeParametersResponse final : Common::BaseCounter<DescribeParametersResponse> {

        /**
         * The token to use when requesting the next set of items.
         */
        std::string nextToken;

        /**
         * Parameters returned by the request.
         */
        std::vector<Parameter> parameters;

      private:

        friend DescribeParametersResponse tag_invoke(boost::json::value_to_tag<DescribeParametersResponse>, boost::json::value const &v) {
            DescribeParametersResponse r;
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            if (Core::Json::AttributeExists(v, "Parameters")) {
                r.parameters = boost::json::value_to<std::vector<Parameter>>(v.at("Parameters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeParametersResponse const &obj) {
            jv = {
                    {"rRegion", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"NextToken", obj.nextToken},
                    {"Parameters", boost::json::value_from(obj.parameters)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_DESCRIBE_PARAMETERS_RESPONSE_H
