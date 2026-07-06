//
// Created by vogje01 on 30/05/2023.
//

#pragma once
// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SSM {

    /**
     * @brief Delete parameter request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteParameterRequest final : Common::BaseCounter<DeleteParameterRequest> {

        /**
         * Parameter name
         */
        std::string name;

      private:

        friend DeleteParameterRequest tag_invoke(boost::json::value_to_tag<DeleteParameterRequest>, boost::json::value const &v) {
            DeleteParameterRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteParameterRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
            };
        }
    };

}// namespace Awsmock::Dto::SSM

