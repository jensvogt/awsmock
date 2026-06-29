//
// Created by vogje01 on 31/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Lambda {

    /**
     * @brief Start all lambda functions request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StartAllLambasRequest final : Common::BaseCounter<StartAllLambasRequest> {

      private:

        friend StartAllLambasRequest tag_invoke(boost::json::value_to_tag<StartAllLambasRequest>, boost::json::value const &v) {
            StartAllLambasRequest r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StartAllLambasRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.region},
                    {"requestId", obj.requestId},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
