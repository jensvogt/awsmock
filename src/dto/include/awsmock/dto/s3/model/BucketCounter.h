//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_BUCKET_COUNTER_H
#define AWSMOCK_DTO_S3_BUCKET_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief AWS S3 bucket counter DTO.
     *
     * @par
     * Used only internally by the frontend.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BucketCounter final : Common::BaseCounter<BucketCounter> {

        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Number of objects keys
         */
        long keys;

        /**
         * Bucket size in bytes
         */
        long size;

        /**
         * Bucket owner
         */
        std::string owner;

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Create timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a BSON document
         *
         * @return BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend BucketCounter tag_invoke(boost::json::value_to_tag<BucketCounter>, boost::json::value const &v) {
            BucketCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.bucketName = v.at("bucketName").as_string();
            r.keys = v.at("keys").as_int64();
            r.size = v.at("size").as_int64();
            r.owner = v.at("owner").as_string();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, BucketCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"bucketName", obj.bucketName},
                    {"keys", obj.keys},
                    {"owner", obj.owner},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_BUCKET_COUNTER_H
