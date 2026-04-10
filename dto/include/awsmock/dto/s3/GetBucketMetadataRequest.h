//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_BUCKET_METADATA_REQUEST_H
#define AWSMOCK_DTO_S3_GET_BUCKET_METADATA_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct GetBucketMetadataRequest final : Common::BaseCounter<GetBucketMetadataRequest> {

        /**
         * Bucket
         */
        std::string bucket;

    private:
        friend GetBucketMetadataRequest tag_invoke(boost::json::value_to_tag<GetBucketMetadataRequest>, boost::json::value const &v) {
            GetBucketMetadataRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetBucketMetadataRequest const &obj) {
            jv = {
                {"region", obj.region},
                {"user", obj.user},
                {"requestId", obj.requestId},
                {"bucket", obj.bucket},
                {"key", obj.bucket},
            };
        }
    };

} // namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_BUCKET_METADATA_REQUEST_H
