//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_METADATA_RESPONSE_H
#define AWSMOCK_DTO_S3_GET_METADATA_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include "awsmock/entity/s3/StorageClass.h"


#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    //using std::chrono::system_clock;

    struct GetMetadataResponse final : Common::BaseCounter<GetMetadataResponse> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * S3 storage class
         */
        std::string storageClass = Database::Entity::S3::StorageClassToString(Database::Entity::S3::StorageClass::STANDARD);

        /**
         * Size
         */
        long size;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Last modified
         */
        system_clock::time_point modified;

        friend GetMetadataResponse tag_invoke(boost::json::value_to_tag<GetMetadataResponse>, boost::json::value const &v) {
            GetMetadataResponse r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            r.md5Sum = Core::Json::GetStringValue(v, "md5Sum");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            r.storageClass = Core::Json::GetStringValue(v, "storageClass");
            r.size = Core::Json::GetLongValue(v, "size");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetMetadataResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.bucket},
                    {"md5Sum", obj.md5Sum},
                    {"contentType", obj.contentType},
                    {"storageClass", obj.storageClass},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_METADATA_RESPONSE_H
