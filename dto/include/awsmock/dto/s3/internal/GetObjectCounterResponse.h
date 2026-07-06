//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/ObjectCounter.h>

namespace Awsmock::Dto::S3 {

    struct GetObjectCounterResponse final : Common::BaseCounter<GetObjectCounterResponse> {

        /**
         * Object counter
         */
        ObjectCounter objectCounter;

      private:

        friend GetObjectCounterResponse tag_invoke(boost::json::value_to_tag<GetObjectCounterResponse>, boost::json::value const &v) {
            GetObjectCounterResponse r;
            r.objectCounter = boost::json::value_to<ObjectCounter>(v.at("objectCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetObjectCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"objectCounter", boost::json::value_from(obj.objectCounter)},
            };
        }
    };
}// namespace Awsmock::Dto::S3
