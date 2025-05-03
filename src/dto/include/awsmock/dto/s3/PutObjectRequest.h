//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_PUT_OBJECT_REQUEST_H
#define AWSMOCK_CORE_DTO_PUT_OBJECT_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct PutObjectRequest final : Common::BaseCounter<PutObjectRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Owner
         */
        std::string owner;

        /**
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * Content type
         */
        long contentLength{};

        /**
         * Checksum algorithm
         */
        std::string checksumAlgorithm;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

      private:

        friend PutObjectRequest tag_invoke(boost::json::value_to_tag<PutObjectRequest>, boost::json::value const &v) {
            PutObjectRequest r;
            r.bucket = v.at("bucket").as_string();
            r.key = v.at("key").as_string();
            r.owner = v.at("owner").as_string();
            r.md5Sum = v.at("md5Sum").as_string();
            r.contentType = v.at("contentType").as_string();
            r.contentLength = v.at("contentLength").as_int64();
            r.checksumAlgorithm = v.at("checksumAlgorithm").as_string();
            r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutObjectRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"owner", obj.owner},
                    {"md5Sum", obj.md5Sum},
                    {"contentType", obj.contentType},
                    {"contentLength", obj.contentLength},
                    {"checksumAlgorithm", obj.checksumAlgorithm},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_CORE_DTO_PUT_OBJECT_REQUEST_H
