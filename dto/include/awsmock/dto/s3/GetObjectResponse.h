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
            r.bucket = v.at("bucket").as_string();
            r.key = v.at("key").as_string();
            r.size = v.at("size").as_int64();
            r.filename = v.at("filename").as_string();
            r.contentType = v.at("contentType").as_string();
            r.md5sum = v.at("md5sum").as_string();
            r.contentType = v.at("contentType").as_string();
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
                    {"metadata", boost::json::value_from(obj.metadata)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_GET_OBJECT_RESPONSE_H
