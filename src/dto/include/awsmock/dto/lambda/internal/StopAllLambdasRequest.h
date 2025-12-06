//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_STOP_ALL_LAMBDAS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_STOP_ALL_LAMBDAS_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Stop all lambda functions request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StopAllLambasRequest final : Common::BaseCounter<StopAllLambasRequest> {

      private:

        friend StopAllLambasRequest tag_invoke(boost::json::value_to_tag<StopAllLambasRequest>, boost::json::value const &v) {
            StopAllLambasRequest r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StopAllLambasRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.region},
                    {"requestId", obj.requestId},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_STOP_ALL_LAMBDAS_REQUEST_H
