//
// Created by vogje01 on 13/10/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETE_BUCKET_LIFECYCLE_REQUEST_H
#define AWSMOCK_CORE_DTO_DELETE_BUCKET_LIFECYCLE_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock include
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/LifecycleRule.h>

namespace AwsMock::Dto::S3 {

    struct DeleteBucketLifecycleRequest final : Common::BaseCounter<DeleteBucketLifecycleRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

      private:

        friend DeleteBucketLifecycleRequest tag_invoke(boost::json::value_to_tag<DeleteBucketLifecycleRequest>, boost::json::value const &v) {
            DeleteBucketLifecycleRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteBucketLifecycleRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_DELETE_BUCKET_LIFECYCLE_REQUEST_H
