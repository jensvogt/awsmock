// Created by vogje01 on 10/1/24.
//

#ifndef AWSMOCK_DTO_S3_MODEL_BUCKET_H
#define AWSMOCK_DTO_S3_MODEL_BUCKET_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// Boost includes
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/version.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/ObjectVersion.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief AWS S3 bucket DTO.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Bucket final : Common::BaseDto<Bucket> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Owner
         */
        std::string owner;

        /**
         * ARM
         */
        std::string arn;

        /**
         * Number of objects keys
         */
        long keys;

        /**
         * Bucket size in bytes
         */
        long size;

        /**
         * Version status
         */
        std::string versionStatus;

        /**
         * List of queue notifications
         */
        std::vector<QueueConfiguration> queueConfigurations;

        /**
         * List of topic notifications
         */
        std::vector<TopicConfiguration> topicConfigurations;

        /**
         * List of lambda notifications
         */
        std::vector<LambdaConfiguration> lambdaConfigurations;

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a JSON object
         *
         * @param jsonObject JSON object
         */
        void FromDocument(const view_or_value<view, value> &jsonObject);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override {
            return ToJson2();
        };

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Bucket const &obj) {
            jv = {
                    {"region", obj.region},
                    {"bucketName", obj.bucketName},
                    {"owner", obj.owner},
                    {"arn", obj.arn},
                    {"keys", obj.keys},
                    {"size", obj.size},
                    {"versionStatus", obj.versionStatus},
                    {"queueConfigurations", boost::json::value_from(obj.queueConfigurations)},
                    {"topicConfigurations", boost::json::value_from(obj.topicConfigurations)},
                    {"lambdaConfigurations", boost::json::value_from(obj.lambdaConfigurations)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };
    //BOOST_DESCRIBE_STRUCT(Bucket, (Common::BaseDto<Bucket>), (region, bucketName, owner, arn, keys, size, versionStatus))
}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_MODEL_BUCKET_H
