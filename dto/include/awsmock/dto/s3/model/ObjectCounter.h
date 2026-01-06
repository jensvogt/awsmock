//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_OBJECT_COUNTER_H
#define AWSMOCK_DTO_S3_OBJECT_COUNTER_H

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
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
         * Object owner
         */
        std::string owner;

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
         * Object body
         */
        std::string body;

        /**
         * S3 storage class
         */
        std::string storageClass;

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

      private:

        friend ObjectCounter tag_invoke(boost::json::value_to_tag<ObjectCounter>, boost::json::value const &v) {
            ObjectCounter r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.bucketName = Core::Json::GetStringValue(v, "bucketName");
            r.key = Core::Json::GetStringValue(v, "key");
            r.owner = Core::Json::GetStringValue(v, "owner");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            r.storageClass = Core::Json::GetStringValue(v, "storageClass");
            r.size = Core::Json::GetLongValue(v, "size");
            r.internalName = Core::Json::GetStringValue(v, "internalName");
            r.body = Core::Json::GetStringValue(v, "body");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ObjectCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"oid", obj.oid},
                    {"bucketName", obj.bucketName},
                    {"key", obj.key},
                    {"owner", obj.owner},
                    {"contentType", obj.contentType},
                    {"storageClass", obj.storageClass},
                    {"size", obj.size},
                    {"internalName", obj.internalName},
                    {"body", obj.body},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };

            // Convert from map to key/value vector
            if (!obj.metadata.empty()) {
                boost::json::object metadataObject;
                for (const auto &[fst, snd]: obj.metadata) {
                    metadataObject[fst] = snd;
                }
                jv.as_object()["metadata"] = metadataObject;
            }
        }
    };
}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OBJECT_COUNTER_H
