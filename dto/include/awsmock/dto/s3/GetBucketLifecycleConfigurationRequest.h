//
// Created by vogje01 on 13/10/2023.
//

#ifndef AWSMOCK_CORE_DTO_GET_BUCKET_LIFECYCLE_CONFIGURATION_REQUEST_H
#define AWSMOCK_CORE_DTO_GET_BUCKET_LIFECYCLE_CONFIGURATION_REQUEST_H

// AwsMock include
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct GetBucketLifecycleConfigurationRequest final : Common::BaseCounter<GetBucketLifecycleConfigurationRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

      private:

        friend GetBucketLifecycleConfigurationRequest tag_invoke(boost::json::value_to_tag<GetBucketLifecycleConfigurationRequest>, boost::json::value const &v) {
            GetBucketLifecycleConfigurationRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetBucketLifecycleConfigurationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_GET_BUCKET_LIFECYCLE_CONFIGURATION_REQUEST_H
