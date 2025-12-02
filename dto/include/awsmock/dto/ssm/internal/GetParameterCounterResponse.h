//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_RESPONSE_H
#define AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/ssm/model/Parameter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Get parameter response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetParameterCounterResponse final : Common::BaseCounter<GetParameterCounterResponse> {

        /**
         * Parameter
         */
        Parameter parameter;

      private:

        friend GetParameterCounterResponse tag_invoke(boost::json::value_to_tag<GetParameterCounterResponse>, boost::json::value const &v) {
            GetParameterCounterResponse r;
            r.parameter = boost::json::value_to<Parameter>(v.at("Parameter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetParameterCounterResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Parameter", boost::json::value_from(obj.parameter)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_RESPONSE_H
