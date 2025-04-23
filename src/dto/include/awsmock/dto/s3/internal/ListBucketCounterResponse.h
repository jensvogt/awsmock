//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
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
            r.region = v.at("region").as_string();
            r.requestId = v.at("requestId").as_string();
            r.user = v.at("user").as_string();
            r.total = v.at("total").as_int64();

            // Bucket counters
            if (v.as_object().contains("bucketCounters")) {
                for (const auto &o: v.at("bucketCounters").as_array()) {
                    r.bucketCounters.emplace_back(boost::json::value_to<BucketCounter>(o));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListBucketCounterResponse const &obj) {
            // Bucket counters
            boost::json::array bucketCounters;
            if (!obj.bucketCounters.empty()) {
                for (const auto &o: obj.bucketCounters) {
                    bucketCounters.emplace_back(boost::json::value_from(o));
                }
            }
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"bucketCounters", bucketCounters},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H
