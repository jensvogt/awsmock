//
// Created by vogje01 on 11/25/23.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Returns a single REST API
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetRestApiCounterRequest final : Common::BaseCounter<GetRestApiCounterRequest> {

        /**
         * REST API name
         */
        std::string name;

      private:

        friend GetRestApiCounterRequest tag_invoke(boost::json::value_to_tag<GetRestApiCounterRequest>, boost::json::value const &v) {
            GetRestApiCounterRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetRestApiCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
