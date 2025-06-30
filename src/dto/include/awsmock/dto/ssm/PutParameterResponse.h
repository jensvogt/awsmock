//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_PUT_PARAMETER_RESPONSE_H
#define AWSMOCK_DTO_SSM_PUT_PARAMETER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Put parameter request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutParameterResponse final : Common::BaseCounter<PutParameterResponse> {

        /**
         * Parameter tier
         */
        std::string tier;

        /**
         * Parameter version
         */
        long version = 0;

      private:

        friend PutParameterResponse tag_invoke(boost::json::value_to_tag<PutParameterResponse>, boost::json::value const &v) {
            PutParameterResponse r;
            r.tier = Core::Json::GetStringValue(v, "Tier");
            r.version = Core::Json::GetLongValue(v, "Version");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutParameterResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Tier", obj.tier},
                    {"Version", obj.version},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_PUT_PARAMETER_RESPONSE_H
