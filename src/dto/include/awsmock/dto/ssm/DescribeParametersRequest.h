//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_DESCRIBE_PARAMETERS_REQUEST_H
#define AWSMOCK_DTO_SSM_DESCRIBE_PARAMETERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/ssm/model/ParameterFilter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Describe parameter request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeParametersRequest final : Common::BaseCounter<DescribeParametersRequest> {

        /**
         * Filters to limit the request results.
         */
        std::vector<ParameterFilter> parameterFilters;

        /**
         * The maximum number of items to return for this call. The call also returns a token that you can specify in a subsequent call to get the next set of results.
         */
        long maxResults{};

        /**
         * The token for the next set of items to return. (You received this token from a previous call.)
         */
        std::string nextToken;

      private:

        friend DescribeParametersRequest tag_invoke(boost::json::value_to_tag<DescribeParametersRequest>, boost::json::value const &v) {
            DescribeParametersRequest r;
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            if (Core::Json::AttributeExists(v, "ParameterFilters")) {
                r.parameterFilters = boost::json::value_to<std::vector<ParameterFilter>>(v.at("ParameterFilters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeParametersRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MaxResults", obj.maxResults},
                    {"NextToken", obj.nextToken},
                    {"ParameterFilters", boost::json::value_from(obj.parameterFilters)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_DESCRIBE_PARAMETERS_REQUEST_H
