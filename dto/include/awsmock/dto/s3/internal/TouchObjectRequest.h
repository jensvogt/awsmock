//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_TOUCH_OBJECT_REQUEST_H
#define AWSMOCK_DTO_S3_TOUCH_OBJECT_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/common/BaseCounter.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct TouchObjectRequest final : Common::BaseCounter<TouchObjectRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

      private:

        friend TouchObjectRequest tag_invoke(boost::json::value_to_tag<TouchObjectRequest>, boost::json::value const &v) {
            TouchObjectRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TouchObjectRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_TOUCH_OBJECT_REQUEST_H
