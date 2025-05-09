//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/BucketCounter.h>

namespace AwsMock::Dto::S3 {

    struct ListBucketCounterResponse final : Common::BaseCounter<ListBucketCounterResponse> {

        /**
         * List of buckets
         */
        std::vector<BucketCounter> bucketCounters;

        /**
         * Total number of buckets
         */
        long long total = 0;

      private:

        friend ListBucketCounterResponse tag_invoke(boost::json::value_to_tag<ListBucketCounterResponse>, boost::json::value const &v) {
            ListBucketCounterResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.bucketCounters = boost::json::value_to<std::vector<BucketCounter>>(v.at("bucketCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListBucketCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"bucketCounters", boost::json::value_from(obj.bucketCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H
