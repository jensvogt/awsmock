//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_PUT_OBJECT_RESPONSE_H
#define AWSMOCK_CORE_DTO_PUT_OBJECT_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct PutObjectResponse final : Common::BaseCounter<PutObjectResponse> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * ETag
         */
        std::string etag = {};

        /**
         * Base64 encoded MD5 sum
         */
        std::string md5Sum = {};

        /**
         * Content length
         */
        long contentLength;

        /**
         * Content type
         */
        std::string contentType = {};

        /**
         * Base64 encoded SHA1 checksum
         */
        std::string sha1Sum = {};

        /**
         * Base64 encoded SHA256 checksum
         */
        std::string sha256sum = {};

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

      private:

        friend PutObjectResponse tag_invoke(boost::json::value_to_tag<PutObjectResponse>, boost::json::value const &v) {
            PutObjectResponse r;
            r.bucket = v.at("bucket").as_string();
            r.key = v.at("key").as_string();
            r.versionId = v.at("versionId").as_string();
            r.etag = v.at("etag").as_string();
            r.md5Sum = v.at("md5Sum").as_string();
            r.contentLength = v.at("contentLength").as_int64();
            r.contentType = v.at("contentType").as_string();
            r.sha1Sum = v.at("sha1Sum").as_string();
            r.sha256sum = v.at("sha256sum").as_string();
            r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutObjectResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"versionId", obj.versionId},
                    {"etag", obj.etag},
                    {"md5Sum", obj.md5Sum},
                    {"contentLength", obj.contentLength},
                    {"contentType", obj.contentType},
                    {"sha1Sum", obj.sha1Sum},
                    {"sha256sum", obj.sha256sum},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_PUT_OBJECT_RESPONSE_H
