//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_REQUEST_H
#define AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/common/SortColumn.h"


#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Get parameter request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetParameterCounterRequest final : Common::BaseCounter<GetParameterCounterRequest> {

        /**
         * Parameter name
         */
        std::string name;

        /**
         * With description
         */
        bool withDescription = false;

      private:

        friend GetParameterCounterRequest tag_invoke(boost::json::value_to_tag<GetParameterCounterRequest>, boost::json::value const &v) {
            GetParameterCounterRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.withDescription = Core::Json::GetBoolValue(v, "withDescription");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetParameterCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"withDescription", obj.withDescription},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_REQUEST_H
