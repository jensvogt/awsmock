//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/s3/model/ObjectCounter.h>

namespace AwsMock::Dto::S3 {

    struct ListObjectCounterResponse final : Common::BaseCounter<ListObjectCounterResponse> {

        /**
         * List of objects
         */
        std::vector<ObjectCounter> objectCounters;

        /**
         * Total number of objects
         */
        long total = 0;

      private:

        friend ListObjectCounterResponse tag_invoke(boost::json::value_to_tag<ListObjectCounterResponse>, boost::json::value const &v) {

            ListObjectCounterResponse r;
            r.region = v.at("region").as_string();
            r.requestId = v.at("requestId").as_string();
            r.user = v.at("user").as_string();
            r.total = v.at("total").as_int64();
            r.objectCounters = boost::json::value_to<std::vector<ObjectCounter>>(v.at("objectCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"objectCounters", boost::json::value_from(obj.objectCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_ALL_OBJECT_RESPONSE_H
