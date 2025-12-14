//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_BUCKET_RESPONSE_H
#define AWSMOCK_DTO_S3_GET_BUCKET_RESPONSE_H

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Get bucket response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetBucketResponse final : Common::BaseCounter<GetBucketResponse> {

        /**
         * ID
         */
        std::string id;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Version status
         */
        std::string versionStatus;

        /**
         * Size in bytes
         */
        long size{};

        /**
         * Object keys count
         */
        long keys{};

        /**
         * Lambda notification configurations
         */
        std::vector<LambdaConfiguration> lambdaConfigurations;

        /**
         * Queue notification configurations
         */
        std::vector<QueueConfiguration> queueConfigurations;

        /**
         * Queue notification configurations
         */
        std::vector<TopicConfiguration> topicConfigurations;

        /**
         * Default metadata
         */
        std::map<std::string, std::string> defaultMetadata;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

      private:

        friend GetBucketResponse tag_invoke(boost::json::value_to_tag<GetBucketResponse>, boost::json::value const &v) {
            GetBucketResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.arn = Core::Json::GetStringValue(v, "arn");
            r.owner = Core::Json::GetStringValue(v, "owner");
            r.versionStatus = Core::Json::GetStringValue(v, "versionStatus");
            r.size = Core::Json::GetLongValue(v, "size");
            r.keys = Core::Json::GetLongValue(v, "keys");
            r.lambdaConfigurations = boost::json::value_to<std::vector<LambdaConfiguration>>(v.at("lambdaConfigurations"));
            r.queueConfigurations = boost::json::value_to<std::vector<QueueConfiguration>>(v.at("queueConfigurations"));
            r.topicConfigurations = boost::json::value_to<std::vector<TopicConfiguration>>(v.at("topicConfigurations"));
            r.defaultMetadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("defaultMetadata"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetBucketResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"id", obj.id},
                    {"bucket", obj.bucket},
                    {"arn", obj.arn},
                    {"owner", obj.owner},
                    {"versionStatus", obj.versionStatus},
                    {"size", obj.size},
                    {"keys", obj.keys},
                    {"lambdaConfigurations", boost::json::value_from(obj.lambdaConfigurations)},
                    {"queueConfigurations", boost::json::value_from(obj.queueConfigurations)},
                    {"topicConfigurations", boost::json::value_from(obj.topicConfigurations)},
                    {"defaultMetadata", boost::json::value_from(obj.defaultMetadata)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_GET_BUCKET_RESPONSE_H
