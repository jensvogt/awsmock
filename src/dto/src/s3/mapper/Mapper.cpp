//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/s3/mapper/Mapper.h>

namespace AwsMock::Dto::S3 {

    ListObjectVersionsResponse Mapper::map(const ListObjectVersionsRequest &request, const std::vector<Database::Entity::S3::Object> &objectList) {

        ListObjectVersionsResponse response;
        response.region = request.region;
        response.name = request.bucket;
        response.maxKeys = request.maxKeys;
        for (const auto &object: objectList) {
            ObjectVersion version = {
                    .key = object.key,
                    .eTag = object.md5sum,
                    .versionId = object.versionId,
                    .storageClass = "STANDARD",
                    .isLatest = false,
                    .size = object.size,
                    .lastModified = object.modified,
            };
            version.owner = {.id = object.owner};

            response.versions.emplace_back(version);
        }
        return response;
    }

    GetBucketResponse Mapper::map(const GetBucketRequest &request, Database::Entity::S3::Bucket &bucket) {
        GetBucketResponse response = {
                .id = bucket.oid,
                .region = bucket.region,
                .bucket = bucket.name,
                .arn = bucket.arn,
                .owner = bucket.owner,
                .versionStatus = BucketVersionStatusToString(bucket.versionStatus),
                .size = bucket.size,
                .keys = bucket.keys,
                .lambdaConfigurations = map(bucket.lambdaNotifications),
                .queueConfigurations = map(bucket.queueNotifications),
                .created = bucket.created,
                .modified = bucket.modified};

        return response;
    }

    Database::Entity::S3::Bucket Mapper::map(const Bucket &bucketDto) {
        Database::Entity::S3::Bucket bucket;
        bucket.region = bucketDto.region;
        bucket.name = bucketDto.bucketName;
        bucket.owner = bucketDto.owner;
        bucket.size = bucketDto.size;
        bucket.keys = bucketDto.keys;
        bucket.versionStatus = Database::Entity::S3::BucketVersionStatusFromString(bucketDto.versionStatus);
        bucket.lambdaNotifications = map(bucketDto.lambdaConfigurations);
        bucket.created = bucketDto.created;
        bucket.modified = bucketDto.modified;
        return bucket;
    }

    Bucket Mapper::map(const Database::Entity::S3::Bucket &bucketEntity) {
        Bucket bucket;
        bucket.region = bucketEntity.region;
        bucket.bucketName = bucketEntity.name;
        bucket.owner = bucketEntity.owner;
        bucket.size = bucketEntity.size;
        bucket.keys = bucketEntity.keys;
        bucket.versionStatus = Database::Entity::S3::BucketVersionStatusToString(bucketEntity.versionStatus);
        bucket.queueConfigurations = map(bucketEntity.queueNotifications);
        bucket.topicConfigurations = map(bucketEntity.topicNotifications);
        bucket.lambdaConfigurations = map(bucketEntity.lambdaNotifications);
        bucket.created = bucketEntity.created;
        bucket.modified = bucketEntity.modified;
        return bucket;
    }

    std::vector<Database::Entity::S3::LambdaNotification> Mapper::map(const std::vector<LambdaConfiguration> &lambdaConfigurationDtos) {
        std::vector<Database::Entity::S3::LambdaNotification> lambdaNotificationEntities;
        for (const auto &lambdaConfigurationDto: lambdaConfigurationDtos) {
            Database::Entity::S3::LambdaNotification lambdaNotification;
            lambdaNotification.id = lambdaConfigurationDto.id;
            lambdaNotification.lambdaArn = lambdaConfigurationDto.lambdaArn;
            //lambdaNotification.events = map(lambdaConfigurationDto.events);
            //lambdaNotification.filterRules = map(lambdaConfigurationDto.filterRules);
            lambdaNotificationEntities.emplace_back(lambdaNotification);
        }
        return lambdaNotificationEntities;
    }

    auto Mapper::map(const std::vector<Database::Entity::S3::LambdaNotification> &lambdaConfigurationEntities) -> std::vector<LambdaConfiguration> {
        std::vector<LambdaConfiguration> lambdaConfigurations;
        for (const auto &[id, events, lambdaArn, filterRules]: lambdaConfigurationEntities) {
            LambdaConfiguration lambdaConfiguration;
            lambdaConfiguration.id = id;
            lambdaConfiguration.lambdaArn = lambdaArn;
            lambdaConfiguration.events = map(events);
            lambdaConfiguration.filterRules = map(filterRules);
            lambdaConfigurations.emplace_back(lambdaConfiguration);
        }
        return lambdaConfigurations;
    }

    std::vector<QueueConfiguration> Mapper::map(const std::vector<Database::Entity::S3::QueueNotification> &queueConfigurationEntities) {
        std::vector<QueueConfiguration> queueConfigurations;
        for (const auto &[id, events, queueArn, filterRules]: queueConfigurationEntities) {
            QueueConfiguration queueConfiguration;
            queueConfiguration.id = id;
            queueConfiguration.queueArn = queueArn;
            queueConfiguration.events = map(events);
            queueConfiguration.filterRules = map(filterRules);
            queueConfigurations.emplace_back(queueConfiguration);
        }
        return queueConfigurations;
    }

    std::vector<TopicConfiguration> Mapper::map(const std::vector<Database::Entity::S3::TopicNotification> &topicConfigurationEntities) {
        std::vector<TopicConfiguration> topicConfigurations;
        for (const auto &[id, events, topicArn, filterRules]: topicConfigurationEntities) {
            TopicConfiguration topicConfiguration;
            topicConfiguration.id = id;
            topicConfiguration.topicArn = topicArn;
            topicConfiguration.events = map(events);
            topicConfiguration.filterRules = map(filterRules);
            topicConfigurations.emplace_back(topicConfiguration);
        }
        return topicConfigurations;
    }

    std::vector<NotificationEventType> Mapper::map(const std::vector<std::string> &eventStrs) {
        std::vector<NotificationEventType> events;
        for (const auto &event: eventStrs) {
            events.emplace_back(EventTypeFromString(event));
        }
        return events;
    }

    std::vector<FilterRule> Mapper::map(const std::vector<Database::Entity::S3::FilterRule> &filterRulesEntities) {
        std::vector<FilterRule> filterRuleDtos;
        for (const auto &[name, value]: filterRulesEntities) {
            FilterRule filterRuleDto;
            filterRuleDto.name = NameTypeFromString(name);
            filterRuleDto.filterValue = value;
            filterRuleDtos.emplace_back(filterRuleDto);
        }
        return filterRuleDtos;
    }

}// namespace AwsMock::Dto::S3