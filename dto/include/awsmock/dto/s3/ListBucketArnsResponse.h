//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_ARNS_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_ARNS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief List S3 bucket ARNs response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListBucketArnsResponse final : Common::BaseCounter<ListBucketArnsResponse> {

        /**
         * List of bucket ARNs
         */
        std::vector<std::string> bucketArns;

      private:

        friend ListBucketArnsResponse tag_invoke(boost::json::value_to_tag<ListBucketArnsResponse>, boost::json::value const &v) {
            ListBucketArnsResponse r;
            r.bucketArns = boost::json::value_to<std::vector<std::string>>(v.at("bucketArns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListBucketArnsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucketArns", boost::json::value_from(obj.bucketArns)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_BUCKET_ARNS_RESPONSE_H
