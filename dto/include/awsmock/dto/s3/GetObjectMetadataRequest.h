//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_METADATA_REQUEST_H
#define AWSMOCK_DTO_S3_GET_METADATA_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct GetObjectMetadataRequest final : Common::BaseCounter<GetObjectMetadataRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

      private:

        friend GetObjectMetadataRequest tag_invoke(boost::json::value_to_tag<GetObjectMetadataRequest>, boost::json::value const &v) {
            GetObjectMetadataRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetObjectMetadataRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.bucket},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_METADATA_REQUEST_H
