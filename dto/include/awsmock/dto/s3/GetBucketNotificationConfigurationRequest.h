//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// AwsMock include
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::S3 {

    struct GetBucketNotificationConfigurationRequest final : Common::BaseCounter<GetBucketNotificationConfigurationRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

      private:

        friend GetBucketNotificationConfigurationRequest tag_invoke(boost::json::value_to_tag<GetBucketNotificationConfigurationRequest>, boost::json::value const &v) {
            GetBucketNotificationConfigurationRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetBucketNotificationConfigurationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
            };
        }
    };
}// namespace Awsmock::Dto::S3
