//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_BUCKET_REQUEST_H
#define AWSMOCK_DTO_S3_GET_BUCKET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Get bucket request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetBucketRequest final : Common::BaseCounter<GetBucketRequest> {

        /**
         * Bucket name
         */
        std::string bucketName;

      private:

        friend GetBucketRequest tag_invoke(boost::json::value_to_tag<GetBucketRequest>, boost::json::value const &v) {
            GetBucketRequest r;
            r.bucketName = Core::Json::GetStringValue(v, "bucketName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetBucketRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucketName", obj.bucketName},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_BUCKET_REQUEST_H
