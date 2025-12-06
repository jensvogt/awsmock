//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_CONTENT_H
#define AWSMOCK_DTO_S3_CONTENT_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/Owner.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief AWS S3 content DTO.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Content final : Common::BaseCounter<Content> {

        /**
         * ETag
         */
        std::string etag;

        /**
         * Key
         */
        std::string key;

        /**
         * Owner
         */
        Owner owner;

        /**
         * Size
         */
        long size = 0;

        /**
         * Checksum algorithms
         */
        std::vector<std::string> checksumAlgorithms;

        /**
         * Storage class
         */
        std::string storageClass;

        /**
         * Last modified
         */
        system_clock::time_point modified;

      private:

        friend Content tag_invoke(boost::json::value_to_tag<Content>, boost::json::value const &v) {
            Content r;
            r.etag = Core::Json::GetStringValue(v, "etag");
            r.key = Core::Json::GetStringValue(v, "key");
            r.size = Core::Json::GetLongValue(v, "size");
            r.storageClass = Core::Json::GetStringValue(v, "storageClass");
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            if (Core::Json::AttributeExists(v, "owner")) {
                r.owner = boost::json::value_to<Owner>(v.at("owner"));
            }
            if (Core::Json::AttributeExists(v, "checksumAlgorithms")) {
                r.checksumAlgorithms = boost::json::value_to<std::vector<std::string>>(v.at("checksumAlgorithms"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Content const &obj) {
            jv = {
                    {"etag", obj.etag},
                    {"key", obj.key},
                    {"size", obj.size},
                    {"storageClass", obj.storageClass},
                    {"checksumAlgorithms", boost::json::value_from(obj.checksumAlgorithms)},
                    {"owner", boost::json::value_from(obj.owner)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_CONTENT_H
