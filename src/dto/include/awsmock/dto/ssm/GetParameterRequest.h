//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_GET_PARAMETER_REQUEST_H
#define AWSMOCK_DTO_SSM_GET_PARAMETER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Get parameter request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetParameterRequest final : Common::BaseCounter<GetParameterRequest> {

        /**
         * Parameter name
         */
        std::string name;

        /**
         * WIth description
         */
        bool withDecryption = false;

      private:

        friend GetParameterRequest tag_invoke(boost::json::value_to_tag<GetParameterRequest>, boost::json::value const &v) {
            GetParameterRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.withDecryption = Core::Json::GetBoolValue(v, "WithDecryption");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetParameterRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"WithDecryption", obj.withDecryption},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_GET_PARAMETER_REQUEST_H
