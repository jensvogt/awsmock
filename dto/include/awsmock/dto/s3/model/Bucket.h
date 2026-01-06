// Created by vogje01 on 10/1/24.
//

#ifndef AWSMOCK_DTO_S3_MODEL_BUCKET_H
#define AWSMOCK_DTO_S3_MODEL_BUCKET_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
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
    struct Bucket final : Common::BaseCounter<Bucket> {

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
        long keys = 0;

        /**
         * Bucket size in bytes
         */
        long size = 0;

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
         * Default metadata
         */
        std::map<std::string, std::string> defaultMetadata;

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend Bucket tag_invoke(boost::json::value_to_tag<Bucket>, boost::json::value const &v) {
            Bucket r;
            r.bucketName = Core::Json::GetStringValue(v, "BucketName");
            r.owner = Core::Json::GetStringValue(v, "Owner");
            r.arn = Core::Json::GetStringValue(v, "Arn");
            r.keys = Core::Json::GetLongValue(v, "Keys");
            r.size = Core::Json::GetLongValue(v, "Size");
            r.versionStatus = Core::Json::GetStringValue(v, "VersionStatus");
            r.created = Core::Json::GetDatetimeValue(v, "Created");
            r.modified = Core::Json::GetDatetimeValue(v, "Modified");
            if (Core::Json::AttributeExists(v, "DefaultMetadata")) {
                r.defaultMetadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("DefaultMetadata"));
            }
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Bucket const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"BucketName", obj.bucketName},
                    {"Owner", obj.owner},
                    {"Arn", obj.arn},
                    {"Keys", obj.keys},
                    {"Size", obj.size},
                    {"VersionStatus", obj.versionStatus},
                    {"QueueConfigurations", boost::json::value_from(obj.queueConfigurations)},
                    {"TopicConfigurations", boost::json::value_from(obj.topicConfigurations)},
                    {"LambdaConfigurations", boost::json::value_from(obj.lambdaConfigurations)},
                    {"DefaultMetadata", boost::json::value_from(obj.defaultMetadata)},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_MODEL_BUCKET_H
