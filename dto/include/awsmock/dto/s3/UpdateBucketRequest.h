//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_UPDATE_BUCKET_REQUEST_H
#define AWSMOCK_DTO_S3_UPDATE_BUCKET_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/Bucket.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Update bucket request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateBucketRequest final : Common::BaseCounter<UpdateBucketRequest> {

        /**
         * Bucket
         */
        Bucket bucket;

      private:

        friend UpdateBucketRequest tag_invoke(boost::json::value_to_tag<UpdateBucketRequest>, boost::json::value const &v) {
            UpdateBucketRequest r;
            r.bucket = boost::json::value_to<Bucket>(v.at("Bucket"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateBucketRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Bucket", boost::json::value_from(obj.bucket)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_UPDATE_BUCKET_REQUEST_H
