//
// Created by vogje01 on 8/2/26.
//

#pragma once

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::S3 {

    struct ListObjectVersionCountersRequest final : Common::BaseCounter<ListObjectVersionCountersRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Object key
         */
        std::string key;

      private:

        friend ListObjectVersionCountersRequest tag_invoke(boost::json::value_to_tag<ListObjectVersionCountersRequest>, boost::json::value const &v) {
            ListObjectVersionCountersRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectVersionCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
            };
        }
    };
}// namespace Awsmock::Dto::S3
