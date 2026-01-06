//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_UPDATE_OBJECT_REQUEST_H
#define AWSMOCK_DTO_S3_UPDATE_OBJECT_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct UpdateObjectRequest final : Common::BaseCounter<UpdateObjectRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * S3 storage class
         */
        std::string storageClass;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

      private:

        friend UpdateObjectRequest tag_invoke(boost::json::value_to_tag<UpdateObjectRequest>, boost::json::value const &v) {

            UpdateObjectRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            r.storageClass = Core::Json::GetStringValue(v, "storageClass");
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateObjectRequest const &obj) {
            jv = {
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"storageClass", obj.storageClass},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_UPDATE_OBJECT_REQUEST_H
