//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_OBJECT_COUNTER_H
#define AWSMOCK_DTO_S3_OBJECT_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief AWS S3 object counter DTO.
     *
     * @par
     * Used only internally by the frontend.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ObjectCounter final : Common::BaseCounter<ObjectCounter> {

        /**
         * Database OID
         */
        std::string oid;

        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Object key
         */
        std::string key;

        /**
         * Content type (mime type)
         */
        std::string contentType;

        /**
         * Object size in bytes
         */
        long size{};

        /**
         * Internal file name
         */
        std::string internalName;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Created timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a BSON document
         *
         * @return BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend ObjectCounter tag_invoke(boost::json::value_to_tag<ObjectCounter>, boost::json::value const &v) {
            ObjectCounter r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.bucketName = Core::Json::GetStringValue(v, "bucketName");
            r.key = Core::Json::GetStringValue(v, "key");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            r.size = Core::Json::GetLongValue(v, "size");
            r.internalName = Core::Json::GetStringValue(v, "internalName");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ObjectCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"oid", obj.oid},
                    {"bucketName", obj.bucketName},
                    {"key", obj.key},
                    {"contentType", obj.contentType},
                    {"size", obj.size},
                    {"internalName", obj.internalName},
                    {"metadata", boost::json::value_from(obj.metadata)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OBJECT_COUNTER_H
