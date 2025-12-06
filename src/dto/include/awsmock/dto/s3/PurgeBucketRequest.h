//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_PURGE_BUCKET_REQUEST_H
#define AWSMOCK_DTO_S3_PURGE_BUCKET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Purge bucket request
     *
     * @par
     * This will delete all object of the given bucket.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PurgeBucketRequest final : Common::BaseCounter<PurgeBucketRequest> {

        /**
         * Bucket name
         */
        std::string bucketName;

      private:

        friend PurgeBucketRequest tag_invoke(boost::json::value_to_tag<PurgeBucketRequest>, boost::json::value const &v) {
            PurgeBucketRequest r;
            r.bucketName = Core::Json::GetStringValue(v, "bucketName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PurgeBucketRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"eTag", obj.bucketName},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PURGE_BUCKET_REQUEST_H
