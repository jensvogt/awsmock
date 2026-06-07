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
     * @brief Delete a REST API request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteRestApiCounterRequest final : Common::BaseCounter<DeleteRestApiCounterRequest> {

        /**
         * REST API name
         */
        std::string name;

      private:

        friend DeleteRestApiCounterRequest tag_invoke(boost::json::value_to_tag<DeleteRestApiCounterRequest>, boost::json::value const &v) {
            DeleteRestApiCounterRequest r;
            if (Core::Json::AttributeExists(v, "name")) {
                r.name = Core::Json::GetStringValue(v, "name");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteRestApiCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", boost::json::value_from(obj.name)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
