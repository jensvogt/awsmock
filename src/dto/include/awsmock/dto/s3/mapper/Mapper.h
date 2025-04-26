//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_S3_MAPPER_H
#define AWSMOCK_DTO_S3_MAPPER_H

// AwsMock includes
#include <awsmock/dto/s3/GetBucketRequest.h>
#include <awsmock/dto/s3/GetBucketResponse.h>
#include <awsmock/dto/s3/ListObjectVersionsRequest.h>
#include <awsmock/dto/s3/ListObjectVersionsResponse.h>
#include <awsmock/dto/s3/model/Bucket.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/LambdaNotification.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/entity/s3/QueueNotification.h>
#include <awsmock/entity/s3/TopicNotification.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a S3 object entity list to a list version response
         *
         * Some values will be pulled over from the request.
         *
         * @param objectList S3 object entity list
         * @param request request struct
         * @return ListObjectVersionsResponse
         * @see ListObjectVersionsResponse
         */
        static ListObjectVersionsResponse map(const ListObjectVersionsRequest &request, const std::vector<Database::Entity::S3::Object> &objectList);

        /**
         * @brief Maps a S3 object entity list to a get bucket request
         *
         * Some values will be pulled over from the request.
         *
         * @param request get bucket request
         * @param bucket bucket entity
         * @return GetBucketResponse
         * @see GetBucketResponse
         */
        static GetBucketResponse map(const GetBucketRequest &request, Database::Entity::S3::Bucket &bucket);

        /**
         * @brief Maps a S3 DTO to a bucket entity
         *
         * Some values will be pulled over from the request.
         *
         * @param bucketDto bucket DTO
         * @return Bucket
         * @see Bucket
         */
        static Database::Entity::S3::Bucket map(const Bucket &bucketDto);

        /**
         * @brief Maps a S3 entity to a bucket DTO
         *
         * @param bucketEntity bucket entity
         * @return Bucket DTO
         * @see Bucket
         */
        static Bucket map(const Database::Entity::S3::Bucket &bucketEntity);

      private:

        /**
         * @brief Maps a S3 queue configuration entity list to a queue configuration DTO
         *
         * @param queueConfigurationEntities queue configuration entities
         * @return QueueConfiguration DTOs
         * @see QueueConfiguration
         */
        static std::vector<QueueConfiguration> map(const std::vector<Database::Entity::S3::QueueNotification> &queueConfigurationEntities);

        /**
         * @brief Maps a S3 topic configuration entity list to a queue configuration DTO
         *
         * @param topicConfigurationEntities topic configuration entities
         * @return TopicConfiguration
         * @see TopicConfiguration
         */
        static std::vector<TopicConfiguration> map(const std::vector<Database::Entity::S3::TopicNotification> &topicConfigurationEntities);

        /**
         * @brief Maps a S3 queue configuration entity list to a queue configuration DTO
         *
         * @param lambdaConfigurationEntities list of lambda configuration entities
         * @return LambdaConfiguration DTOs
         * @see LambdaConfiguration
         */
        static std::vector<LambdaConfiguration> map(const std::vector<Database::Entity::S3::LambdaNotification> &lambdaConfigurationEntities);

        /**
         * @brief Maps event string to event types.
         *
         * @param eventStrs event strings
         * @return NotificationEventTypes
         */
        static std::vector<NotificationEventType> map(const std::vector<std::string> &eventStrs);

        /**
         * @brief Map filter rules entities to file rules DTOs
         *
         * @param filterRulesEntities filter rules entities
         * @return filter rules DTOs
         */
        static std::vector<FilterRule> map(const std::vector<Database::Entity::S3::FilterRule> &filterRulesEntities);

        /**
         * @brief Maps a S3 queue configuration DTO list to a queue configuration entity
         *
         * @param lambdaConfiguration list of lambda configuration
         * @return list of LambdaConfiguration
         * @see LambdaConfiguration
         */
        static std::vector<Database::Entity::S3::LambdaNotification> map(const std::vector<LambdaConfiguration> &lambdaConfiguration);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_MAPPER_H
