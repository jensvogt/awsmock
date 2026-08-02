//
// Created by vogje01 on 8/2/26.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/ObjectVersionCounter.h>

namespace Awsmock::Dto::S3 {

    struct ListObjectVersionCountersResponse final : Common::BaseCounter<ListObjectVersionCountersResponse> {

        /**
         * List of object versions, most recently modified first
         */
        std::vector<ObjectVersionCounter> versionCounters;

        /**
         * Total number of versions
         */
        long total = 0;

      private:

        friend ListObjectVersionCountersResponse tag_invoke(boost::json::value_to_tag<ListObjectVersionCountersResponse>, boost::json::value const &v) {

            ListObjectVersionCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "versionCounters")) {
                r.versionCounters = boost::json::value_to<std::vector<ObjectVersionCounter>>(v.at("versionCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectVersionCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"versionCounters", boost::json::value_from(obj.versionCounters)},
            };
        }
    };
}// namespace Awsmock::Dto::S3
