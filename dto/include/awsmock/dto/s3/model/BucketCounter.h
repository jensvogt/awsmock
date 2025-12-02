//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_BUCKET_COUNTER_H
#define AWSMOCK_DTO_S3_BUCKET_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
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
         * Bucket ARN
         */
        std::string bucketArn;

        /**
         * Number of objects keys
         */
        long keys = 0;

        /**
         * Bucket size in bytes
         */
        long size = 0;

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "bucketName", bucketName);
                Core::Bson::BsonUtils::SetStringValue(document, "bucketArn", bucketArn);
                Core::Bson::BsonUtils::SetLongValue(document, "keys", keys);
                Core::Bson::BsonUtils::SetLongValue(document, "size", size);
                Core::Bson::BsonUtils::SetStringValue(document, "owner", owner);
                Core::Bson::BsonUtils::SetDateValue(document, "created", created);
                Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend BucketCounter tag_invoke(boost::json::value_to_tag<BucketCounter>, boost::json::value const &v) {
            BucketCounter r;
            r.bucketName = Core::Json::GetStringValue(v, "bucketName");
            r.bucketArn = Core::Json::GetStringValue(v, "bucketArn");
            r.keys = Core::Json::GetLongValue(v, "keys");
            r.size = Core::Json::GetLongValue(v, "size");
            r.owner = Core::Json::GetStringValue(v, "owner");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, BucketCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"bucketName", obj.bucketName},
                    {"bucketArn", obj.bucketArn},
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
