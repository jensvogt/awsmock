//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_GET_PARAMETER_RESPONSE_H
#define AWSMOCK_DTO_SSM_GET_PARAMETER_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/ssm/model/Parameter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Get parameter response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetParameterResponse final : Common::BaseCounter<GetParameterResponse> {

        /**
         * Parameter
         */
        Parameter parameter;

      private:

        friend GetParameterResponse tag_invoke(boost::json::value_to_tag<GetParameterResponse>, boost::json::value const &v) {
            GetParameterResponse r;
            r.parameter = boost::json::value_to<Parameter>(v.at("Parameter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetParameterResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Parameter", boost::json::value_from(obj.parameter)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_GET_PARAMETER_RESPONSE_H
