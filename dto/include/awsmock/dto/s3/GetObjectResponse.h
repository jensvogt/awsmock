//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_OBJECT_RESPONSE_H
#define AWSMOCK_DTO_S3_GET_OBJECT_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief Get object response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetObjectResponse final : Common::BaseCounter<GetObjectResponse> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Content length
         */
        long size;

        /**
         * File name
         */
        std::string filename;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * MD5 sum
         */
        std::string md5sum;

        /**
         * S3 storage class
         */
        std::string storageClass;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

      private:

        friend GetObjectResponse tag_invoke(boost::json::value_to_tag<GetObjectResponse>, boost::json::value const &v) {
            GetObjectResponse r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            r.size = Core::Json::GetLongValue(v, "size");
            r.filename = Core::Json::GetStringValue(v, "filename");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            r.md5sum = Core::Json::GetStringValue(v, "md5sum");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            r.storageClass = Core::Json::GetStringValue(v, "storageClass");
            r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetObjectResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"size", obj.size},
                    {"filename", obj.filename},
                    {"md5sum", obj.md5sum},
                    {"contentLength", obj.size},
                    {"contentType", obj.contentType},
                    {"storageClass", obj.storageClass},
                    {"metadata", boost::json::value_from(obj.metadata)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_GET_OBJECT_RESPONSE_H
