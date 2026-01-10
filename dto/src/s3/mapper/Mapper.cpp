//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/s3/mapper/Mapper.h>

#include "awsmock/dto/s3/PutBucketLifecycleConfigurationRequest.h"

namespace AwsMock::Dto::S3 {

    ListObjectVersionsResponse Mapper::map(const ListObjectVersionsRequest &request, const std::vector<Database::Entity::S3::Object> &objectList) {

        ListObjectVersionsResponse response;
        response.region = request.region;
        response.name = request.bucket;
        response.maxKeys = request.maxKeys;
        for (const auto &object: objectList) {
            ObjectVersion version;
            version.key = object.key;
            version.eTag = object.md5sum;
            version.versionId = object.versionId;
            version.storageClass = "STANDARD";
            version.isLatest = false;
            version.size = object.size;
            version.lastModified = object.modified;
            version.owner.id = object.owner;
            response.versions.emplace_back(version);
        }
        return response;
    }

    GetBucketResponse Mapper::map(const GetBucketRequest &request, const Database::Entity::S3::Bucket &bucket) {
        GetBucketResponse response;
        response.region = request.region;
        response.user = request.user;
        response.requestId = request.requestId;
        response.id = bucket.oid;
        response.region = bucket.region;
        response.bucket = bucket.name;
        response.arn = bucket.arn;
        response.owner = bucket.owner;
        response.versionStatus = BucketVersionStatusToString(bucket.versionStatus);
        response.size = bucket.size;
        response.keys = bucket.keys;
        response.lambdaConfigurations = map(bucket.lambdaNotifications);
        response.queueConfigurations = map(bucket.queueNotifications);
        response.topicConfigurations = map(bucket.topicNotifications);
        response.defaultMetadata = bucket.defaultMetadata;
        response.created = bucket.created;
        response.modified = bucket.modified;
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
            lambdaNotification.events = map(lambdaConfigurationDto.events);
            lambdaNotification.filterRules = map(lambdaConfigurationDto.filterRules);
            lambdaNotificationEntities.emplace_back(lambdaNotification);
        }
        return lambdaNotificationEntities;
    }

    auto Mapper::map(const std::vector<Database::Entity::S3::LambdaNotification> &lambdaConfigurationEntities) -> std::vector<LambdaConfiguration> {
        std::vector<LambdaConfiguration> lambdaConfigurations;
        for (const auto &lambdaConfigurationEntity: lambdaConfigurationEntities) {
            lambdaConfigurations.emplace_back(map(lambdaConfigurationEntity));
        }
        return lambdaConfigurations;
    }

    auto Mapper::map(const std::vector<NotificationEventType> &lambdaNotificationEventTypeEntities) -> std::vector<std::string> {
        std::vector<std::string> eventDtos;
        for (const auto &lambdaNotificationEventTypeEntity: lambdaNotificationEventTypeEntities) {
            eventDtos.emplace_back(EventTypeToString(lambdaNotificationEventTypeEntity));
        }
        return eventDtos;
    }

    auto Mapper::map(const std::vector<FilterRule> &filterRulesDtos) -> std::vector<Database::Entity::S3::FilterRule> {
        std::vector<Database::Entity::S3::FilterRule> fileRulesEntities;
        for (const auto &filterRulesDto: filterRulesDtos) {
            Database::Entity::S3::FilterRule filterRuleEntity;
            filterRuleEntity.region = filterRulesDto.region;
            filterRuleEntity.name = NameTypeToString(filterRulesDto.name);
            filterRuleEntity.value = filterRulesDto.filterValue;
            fileRulesEntities.emplace_back(filterRuleEntity);
        }
        return fileRulesEntities;
    }

    auto Mapper::map(const Database::Entity::S3::LambdaNotification &lambdaConfigurationEntity) -> LambdaConfiguration {
        LambdaConfiguration lambdaConfiguration;
        lambdaConfiguration.id = lambdaConfigurationEntity.id;
        lambdaConfiguration.lambdaArn = lambdaConfigurationEntity.lambdaArn;
        lambdaConfiguration.events = map(lambdaConfigurationEntity.events);
        lambdaConfiguration.filterRules = map(lambdaConfigurationEntity.filterRules);
        return lambdaConfiguration;
    }

    std::vector<QueueConfiguration> Mapper::map(const std::vector<Database::Entity::S3::QueueNotification> &queueConfigurationEntities) {
        std::vector<QueueConfiguration> queueConfigurations;
        for (const auto &queueConfigurationEntity: queueConfigurationEntities) {
            QueueConfiguration queueConfiguration;
            queueConfiguration.id = queueConfigurationEntity.id;
            queueConfiguration.queueArn = queueConfigurationEntity.queueArn;
            queueConfiguration.events = map(queueConfigurationEntity.events);
            queueConfiguration.filterRules = map(queueConfigurationEntity.filterRules);
            queueConfigurations.emplace_back(queueConfiguration);
        }
        return queueConfigurations;
    }

    std::vector<TopicConfiguration> Mapper::map(const std::vector<Database::Entity::S3::TopicNotification> &topicConfigurationEntities) {
        std::vector<TopicConfiguration> topicConfigurations;
        for (const auto &topicConfigurationEntity: topicConfigurationEntities) {
            TopicConfiguration topicConfiguration;
            topicConfiguration.id = topicConfigurationEntity.id;
            topicConfiguration.topicArn = topicConfigurationEntity.topicArn;
            topicConfiguration.events = map(topicConfigurationEntity.events);
            topicConfiguration.filterRules = map(topicConfigurationEntity.filterRules);
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
        for (const auto &filterRulesEntity: filterRulesEntities) {
            FilterRule filterRuleDto;
            filterRuleDto.name = NameTypeFromString(filterRulesEntity.name);
            filterRuleDto.filterValue = filterRulesEntity.value;
            filterRuleDtos.emplace_back(filterRuleDto);
        }
        return filterRuleDtos;
    }

    EventNotification Mapper::map(const std::string &notificationId, Bucket &bucket, Object &object, const std::string &event) {

        NotificationBucket notificationBucket;
        notificationBucket.name = bucket.bucketName;
        notificationBucket.arn = bucket.arn;
        notificationBucket.ownerIdentity.displayName = bucket.owner;

        S3 s3;
        s3.configurationId = notificationId;
        s3.bucket = notificationBucket;
        s3.object.key = object.key;
        s3.object.etag = object.etag;
        s3.object.size = object.size;
        s3.object.versionId = object.versionId;

        Record record;
        record.region = bucket.region;
        record.eventName = event;
        record.s3 = s3;

        EventNotification eventNotification;
        eventNotification.records.push_back(record);

        return eventNotification;
    }

    Database::Entity::S3::LifecycleTransition Mapper::map(const LifecycleTransition &transition) {
        Database::Entity::S3::LifecycleTransition transitionEntity;
        transitionEntity.days = transition.days;
        transitionEntity.date = transition.date;
        transitionEntity.storageClass = Database::Entity::S3::StorageClassFromString(StorageClassToString(transition.storageClass));
        return transitionEntity;
    }

    Database::Entity::S3::LifecycleConfiguration Mapper::map(const LifecycleRule &rule) {
        Database::Entity::S3::LifecycleConfiguration configuration;
        configuration.id = rule.id;
        configuration.status = Database::Entity::S3::LifeCycleStatusFromString(LifeCycleStatusToString(rule.status));
        for (const auto &transition: rule.transitions) {
            configuration.transitions.emplace_back(map(transition));
        }
        return configuration;
    }

    std::vector<Database::Entity::S3::LifecycleConfiguration> Mapper::map(const PutBucketLifecycleConfigurationRequest &request) {
        std::vector<Database::Entity::S3::LifecycleConfiguration> lifecycleConfigurations;
        for (const auto &rule: request.rules) {
            lifecycleConfigurations.emplace_back(map(rule));
        }
        return lifecycleConfigurations;
    }

    LifecycleTransition Mapper::map(const Database::Entity::S3::LifecycleTransition &transition) {
        LifecycleTransition transitionDto;
        transitionDto.days = transition.days;
        transitionDto.date = transition.date;
        transitionDto.storageClass = StorageClassFromString(Database::Entity::S3::StorageClassToString(transition.storageClass));
        return transitionDto;
    }

    LifecycleRule Mapper::map(const Database::Entity::S3::LifecycleConfiguration &configuration) {
        LifecycleRule rule;
        rule.id = configuration.id;
        rule.prefix = configuration.prefix;
        rule.status = LifeCycleStatusFromString(LifeCycleStatusToString(configuration.status));
        if (!configuration.transitions.empty()) {
            for (const auto &transition: configuration.transitions) {
                rule.transitions.emplace_back(map(transition));
            }
        }
        return rule;
    }

    std::vector<LifecycleRule> Mapper::map(const std::vector<Database::Entity::S3::LifecycleConfiguration> &configurations) {
        std::vector<LifecycleRule> rules;
        for (const auto &configuration: configurations) {
            rules.emplace_back(map(configuration));
        }
        return rules;
    }

}// namespace AwsMock::Dto::S3