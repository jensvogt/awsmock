//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETE_BUCKET_REQUEST_H
#define AWSMOCK_CORE_DTO_DELETE_BUCKET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Delete bucket request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteBucketRequest final : Common::BaseCounter<DeleteBucketRequest> {

        /**
         * Region
         */
        std::string owner;

        /**
         * Bucket
         */
        std::string bucket;

      private:

        friend DeleteBucketRequest tag_invoke(boost::json::value_to_tag<DeleteBucketRequest>, boost::json::value const &v) {
            DeleteBucketRequest r;
            r.bucket = Core::Json::GetStringValue(v, "Bucket");
            r.owner = Core::Json::GetStringValue(v, "Owner");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteBucketRequest const &obj) {
            jv = {
                    {"Bucket", obj.bucket},
                    {"Owner", obj.owner},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_DELETE_BUCKET_REQUEST_H
