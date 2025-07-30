//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_REQUEST_H
#define AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Get parameter counter request.
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
        bool withDecryption = false;

      private:

        friend GetParameterCounterRequest tag_invoke(boost::json::value_to_tag<GetParameterCounterRequest>, boost::json::value const &v) {
            GetParameterCounterRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.withDecryption = Core::Json::GetBoolValue(v, "WithDecryption");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetParameterCounterRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"WithDescription", obj.withDecryption},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_GET_PARAMETER_COUNTER_REQUEST_H
