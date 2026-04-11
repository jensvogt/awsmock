//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_S3_MAPPER_H
#define AWSMOCK_DTO_S3_MAPPER_H

// AwsMock includes
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/s3/CreateBucketRequest.h>
#include <awsmock/dto/s3/CreateBucketResponse.h>
#include <awsmock/dto/s3/GetObjectMetadataResponse.h>
#include <awsmock/dto/s3/ListObjectVersionsResponse.h>
#include <awsmock/dto/s3/internal/GetBucketResponse.h>
#include <awsmock/dto/s3/model/Bucket.h>
#include <awsmock/dto/s3/model/BucketCounter.h>
#include <awsmock/dto/s3/model/EventNotification.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/LifecycleStatus.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>
#include <awsmock/dto/sqs/model/EventNotification.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/LambdaNotification.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/entity/s3/QueueNotification.h>
#include <awsmock/entity/s3/TopicNotification.h>

namespace AwsMock::Dto::S3 {

    class FilterRuleMapper : public StaticMapper<FilterRuleMapper, Database::Entity::S3::FilterRule, FilterRule> {
      public:

        static FilterRule toDto(const Database::Entity::S3::FilterRule &e) {
            FilterRule d;
            d.region = e.region;
            d.name = NameTypeFromString(e.name);
            d.filterValue = e.value;
            return d;
        }

        static Database::Entity::S3::FilterRule toEntity(const FilterRule &d) {
            Database::Entity::S3::FilterRule e;
            e.region = d.region;
            e.name = NameTypeToString(d.name);
            e.value = d.filterValue;
            return e;
        }
    };

    class QueueConfigurationMapper : public StaticMapper<QueueConfigurationMapper, Database::Entity::S3::QueueNotification, QueueConfiguration> {

      public:

        static QueueConfiguration toDto(const Database::Entity::S3::QueueNotification &e) {
            QueueConfiguration d;
            d.region = e.region;
            d.id = e.id;
            d.queueArn = e.queueArn;
            d.filterRules = FilterRuleMapper::toDtoList(e.filterRules);
            for (const auto &event: e.events) {
                d.events.emplace_back(EventTypeFromString(event));
            }
            return d;
        }

        static Database::Entity::S3::QueueNotification toEntity(const QueueConfiguration &d) {
            Database::Entity::S3::QueueNotification e;
            e.region = d.region;
            e.id = d.id;
            e.queueArn = d.queueArn;
            e.filterRules = FilterRuleMapper::toEntityList(d.filterRules);
            for (const auto &event: d.events) {
                e.events.emplace_back(EventTypeToString(event));
            }
            return e;
        }
    };

    class TopicConfigurationMapper : public StaticMapper<TopicConfigurationMapper, Database::Entity::S3::TopicNotification, TopicConfiguration> {

      public:

        static TopicConfiguration toDto(const Database::Entity::S3::TopicNotification &e) {
            TopicConfiguration d;
            d.region = e.region;
            d.id = e.id;
            d.topicArn = e.topicArn;
            d.filterRules = FilterRuleMapper::toDtoList(e.filterRules);
            for (const auto &event: e.events) {
                d.events.emplace_back(EventTypeFromString(event));
            }
            return d;
        }

        static Database::Entity::S3::TopicNotification toEntity(const TopicConfiguration &d) {
            Database::Entity::S3::TopicNotification e;
            e.region = d.region;
            e.id = d.id;
            e.topicArn = d.topicArn;
            e.filterRules = FilterRuleMapper::toEntityList(d.filterRules);
            for (const auto &event: d.events) {
                e.events.emplace_back(EventTypeToString(event));
            }
            return e;
        }
    };

    class LambdaConfigurationMapper : public StaticMapper<LambdaConfigurationMapper, Database::Entity::S3::LambdaNotification, LambdaConfiguration> {

      public:

        static LambdaConfiguration toDto(const Database::Entity::S3::LambdaNotification &e) {
            LambdaConfiguration d;
            d.region = e.region;
            d.id = e.id;
            d.lambdaArn = e.lambdaArn;
            d.filterRules = FilterRuleMapper::toDtoList(e.filterRules);
            for (const auto &event: e.events) {
                d.events.emplace_back(EventTypeFromString(event));
            }
            return d;
        }

        static Database::Entity::S3::LambdaNotification toEntity(const LambdaConfiguration &d) {
            Database::Entity::S3::LambdaNotification e;
            e.region = d.region;
            e.id = d.id;
            e.lambdaArn = d.lambdaArn;
            e.filterRules = FilterRuleMapper::toEntityList(d.filterRules);
            for (const auto &event: d.events) {
                e.events.emplace_back(EventTypeToString(event));
            }
            return e;
        }
    };

    class LifecycleTransitionMapper : public StaticMapper<LifecycleTransitionMapper, Database::Entity::S3::LifecycleTransition, LifecycleTransition> {
      public:

        static LifecycleTransition toDto(const Database::Entity::S3::LifecycleTransition &e) {
            LifecycleTransition d;
            d.region = e.region;
            d.date = e.date;
            d.days = e.days;
            d.storageClass = StorageClassFromString(StorageClassToString(e.storageClass));
            return d;
        }

        static Database::Entity::S3::LifecycleTransition toEntity(const LifecycleTransition &d) {
            Database::Entity::S3::LifecycleTransition e;
            e.region = d.region;
            e.days = d.days;
            e.date = d.date;
            e.storageClass = Database::Entity::S3::StorageClassFromString(StorageClassToString(d.storageClass));
            return e;
        }
    };

    class LifecycleMapper : public StaticMapper<LifecycleMapper, Database::Entity::S3::Bucket, LifecycleRule> {
      public:

        static LifecycleRule toDto(const Database::Entity::S3::LifecycleConfiguration &e) {
            LifecycleRule d;
            d.region = e.region;
            d.prefix = e.prefix;
            d.transitions = LifecycleTransitionMapper::toDtoList(e.transitions);
            d.status = LifeCycleStatusFromString(Database::Entity::S3::LifeCycleStatusToString(e.status));
            return d;
        }

        static Database::Entity::S3::LifecycleConfiguration toEntity(const LifecycleRule &d) {
            Database::Entity::S3::LifecycleConfiguration e;
            e.region = d.region;
            e.prefix = d.prefix;
            e.status = Database::Entity::S3::LifeCycleStatusFromString(LifeCycleStatusToString(d.status));
            e.transitions = LifecycleTransitionMapper::toEntityList(d.transitions);
            return e;
        }
    };

    class BucketCounterMapper : public StaticMapper<BucketCounterMapper, Database::Entity::S3::Bucket, BucketCounter> {

      public:

        static BucketCounter toDto(const Database::Entity::S3::Bucket &e) {
            BucketCounter d;
            d.region = e.region;
            d.bucketArn = e.arn;
            d.bucketName = e.name;
            d.keys = e.keys;
            d.size = e.size;
            d.owner = e.owner;
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }
    };

    class BucketCreateRequestMapper : public StaticMapper<BucketCreateRequestMapper, Database::Entity::S3::Bucket, CreateBucketRequest> {

      public:

        static Database::Entity::S3::Bucket toEntity(const CreateBucketRequest &r) {
            const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
            Database::Entity::S3::Bucket b;
            b.region = r.region;
            b.owner = r.owner;
            b.name = r.name;
            b.arn = Core::AwsUtils::CreateS3BucketArn(r.region, accountId, r.name);
            return b;
        }
    };

    class BucketCreateResponseMapper : public StaticMapper<BucketCreateResponseMapper, Database::Entity::S3::Bucket, CreateBucketResponse> {

      public:

        static CreateBucketResponse toDto(const Database::Entity::S3::Bucket &b) {
            CreateBucketResponse r;
            r.region = b.region;
            r.user = b.owner;
            r.arn = b.arn;
            return r;
        }
    };

    class BucketGetResponseMapper : public StaticMapper<BucketGetResponseMapper, Database::Entity::S3::Bucket, GetBucketResponse> {

      public:

        static GetBucketResponse toDto(const Database::Entity::S3::Bucket &b) {
            GetBucketResponse r;
            r.region = b.region;
            r.user = b.owner;
            r.owner = b.owner;
            r.arn = b.arn;
            r.bucket = b.name;
            r.size = b.size;
            r.keys = b.keys;
            r.defaultMetadata = b.defaultMetadata;
            r.created = b.created;
            r.modified = b.modified;
            r.versionStatus = BucketVersionStatusToString(b.versionStatus);
            r.queueConfigurations = QueueConfigurationMapper::toDtoList(b.queueNotifications);
            r.topicConfigurations = TopicConfigurationMapper::toDtoList(b.topicNotifications);
            r.lambdaConfigurations = LambdaConfigurationMapper::toDtoList(b.lambdaNotifications);
            return r;
        }
    };

    class GetBucketMetadataResponseMapper : public StaticMapper<GetBucketMetadataResponseMapper, Database::Entity::S3::Bucket, GetObjectMetadataResponse> {

      public:

        static GetObjectMetadataResponse toDto(const Database::Entity::S3::Bucket &e) {
            GetObjectMetadataResponse d;
            d.region = e.region;
            d.bucket = e.name;
            d.size = e.size;
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }
    };

    class GetObjectMetadataResponseMapper : public StaticMapper<GetObjectMetadataResponseMapper, Database::Entity::S3::Object, GetObjectMetadataResponse> {

      public:

        static GetObjectMetadataResponse toDto(const Database::Entity::S3::Object &e) {
            GetObjectMetadataResponse d;
            d.region = e.region;
            d.bucket = e.bucket;
            d.key = e.key;
            d.md5Sum = e.md5sum;
            d.contentType = e.contentType;
            d.size = e.size;
            d.metadata = e.metadata;
            d.storageClass = Database::Entity::S3::StorageClassToString(e.storageClass);
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }
    };

    class ListVersionsResponseMapper : public StaticMapper<ListVersionsResponseMapper, std::vector<Database::Entity::S3::Object>, ListObjectVersionsResponse> {

      public:

        static ListObjectVersionsResponse toDto(const std::vector<Database::Entity::S3::Object> &e) {
            ListObjectVersionsResponse d;
            for (const auto &object: e) {
                ObjectVersion version;
                version.key = object.key;
                version.eTag = object.md5sum;
                version.versionId = object.versionId;
                version.storageClass = "STANDARD";
                version.isLatest = false;
                version.size = object.size;
                version.lastModified = object.modified;
                version.owner.id = object.owner;
                d.versions.emplace_back(version);
            }
            return d;
        }
    };

    class BucketMapper : public StaticMapper<BucketMapper, Database::Entity::S3::Bucket, Bucket> {

      public:

        static Bucket toDto(const Database::Entity::S3::Bucket &e) {
            Bucket d;
            d.region = e.region;
            d.arn = e.arn;
            d.bucketName = e.name;
            d.queueConfigurations = QueueConfigurationMapper::toDtoList(e.queueNotifications);
            d.topicConfigurations = TopicConfigurationMapper::toDtoList(e.topicNotifications);
            d.lambdaConfigurations = LambdaConfigurationMapper::toDtoList(e.lambdaNotifications);
            d.defaultMetadata = e.defaultMetadata;
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }

        static Database::Entity::S3::Bucket toEntity(const Bucket &d) {
            Database::Entity::S3::Bucket e;
            e.region = d.region;
            e.name = d.bucketName;
            e.arn = d.arn;
            e.owner = d.owner;
            e.arn = d.arn;
            e.size = d.size;
            e.keys = d.keys;
            e.queueNotifications = QueueConfigurationMapper::toEntityList(d.queueConfigurations);
            e.topicNotifications = TopicConfigurationMapper::toEntityList(d.topicConfigurations);
            e.lambdaNotifications = LambdaConfigurationMapper::toEntityList(d.lambdaConfigurations);
            e.defaultMetadata = d.defaultMetadata;
            e.created = d.created;
            e.modified = d.modified;
            return e;
        }
    };

    class ObjectMapper : public StaticMapper<ObjectMapper, Database::Entity::S3::Object, Object> {

      public:

        static Object toDto(const Database::Entity::S3::Object &e) {
            Object d;
            d.region = e.region;
            d.key = e.key;
            d.size = e.size;
            d.contentType = e.contentType;
            d.etag = e.md5sum;
            d.versionId = e.versionId;
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }

        static Database::Entity::S3::Object toEntity(const Object &d) {
            Database::Entity::S3::Object e;
            e.region = d.region;
            e.key = d.key;
            e.size = d.size;
            e.contentType = d.contentType;
            e.md5sum = d.etag;
            e.versionId = d.versionId;
            e.created = d.created;
            e.modified = d.modified;
            return e;
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_MAPPER_H
