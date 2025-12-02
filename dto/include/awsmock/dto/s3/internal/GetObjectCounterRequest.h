//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_OBJECT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_GET_OBJECT_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct GetObjectCounterRequest final : Common::BaseCounter<GetObjectCounterRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Object OID
         */
        std::string oid;

      private:

        friend GetObjectCounterRequest tag_invoke(boost::json::value_to_tag<GetObjectCounterRequest>, boost::json::value const &v) {
            GetObjectCounterRequest r;
            r.region = Core::Json::GetStringValue(v, "region");
            r.oid = Core::Json::GetStringValue(v, "oid");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetObjectCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"oid", obj.oid},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_GET_OBJECT_COUNTER_REQUEST_H
