//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_OBJECT_COUNTER_H
#define AWSMOCK_DTO_S3_OBJECT_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
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
         * AWS region
         */
        std::string region;

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
        long size;

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
            r.region = v.at("region").as_string();
            r.oid = v.at("oid").as_string();
            r.bucketName = v.at("bucketName").as_string();
            r.key = v.at("key").as_string();
            r.contentType = v.at("contentType").as_string();
            r.size = v.at("size").as_int64();
            r.internalName = v.at("internalName").as_string();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());

            // Metadata
            if (v.as_object().contains("metadata")) {
                for (const auto &m: v.at("metadata").as_object()) {
                    r.metadata[m.key()] = m.value().as_string().data();
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ObjectCounter const &obj) {

            // Metadata
            boost::json::object metadata;
            if (!obj.metadata.empty()) {
                for (const auto &[fst, snd]: obj.metadata) {
                    metadata[fst] = snd;
                }
            }
            jv = {
                    {"region", obj.region},
                    {"oid", obj.oid},
                    {"bucketName", obj.bucketName},
                    {"key", obj.key},
                    {"contentType", obj.contentType},
                    {"size", obj.size},
                    {"internalName", obj.internalName},
                    {"metadata", metadata},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OBJECT_COUNTER_H
