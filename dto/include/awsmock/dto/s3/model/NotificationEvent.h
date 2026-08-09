//
// Created by JVO on 29.04.2024.
//

#pragma once

// C++ standard includes
#include <string>

namespace Awsmock::Dto::S3 {

    /**
     * @brief S3 notification events
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class NotificationEventType {
        REDUCED_REDUNDANCY_LOST_OBJECT,
        OBJECT_CREATED,
        OBJECT_REMOVED,
        OBJECT_RESTORED,
        REPLICATION,
        OBJECT_RESTORE,
        OBJECT_LIFECYCLE_TRANSITION,
        INTELLIGENT_TIERING,
        OBJECT_ACL,
        LIFECYCLE_EXPIRATION,
        OBJECT_TAGGING
    };

    static std::map<NotificationEventType, std::string> EventTypeNames{
            {NotificationEventType::REDUCED_REDUNDANCY_LOST_OBJECT, "s3:ReducedRedundancyLostObject"},
            {NotificationEventType::OBJECT_CREATED, "s3:ObjectCreated:*"},
            {NotificationEventType::OBJECT_REMOVED, "s3:ObjectRemoved:*"},
            {NotificationEventType::OBJECT_RESTORED, "s3:ObjectRestore:*"},
            {NotificationEventType::REPLICATION, "s3:Replication:*"},
            {NotificationEventType::OBJECT_RESTORE, "s3:ObjectRestore:*"},
            {NotificationEventType::OBJECT_LIFECYCLE_TRANSITION, "s3:LifecycleTransition"},
            {NotificationEventType::INTELLIGENT_TIERING, "s3:IntelligentTiering"},
            {NotificationEventType::OBJECT_ACL, "s3:ObjectAcl:Put"},
            {NotificationEventType::LIFECYCLE_EXPIRATION, "s3:LifecycleExpiration:*"},
            {NotificationEventType::OBJECT_TAGGING, "s3:ObjectTagging:*"},
    };

    /**
     * @brief Category prefix used to match a configured event (e.g. "s3:ObjectCreated:*") against any
     * specific sub-event AWS might send (e.g. "s3:ObjectCreated:Put"), since awsmock does not distinguish
     * between the individual sub-events within a category.
     */
    static std::map<NotificationEventType, std::string> EventTypePrefixes{
            {NotificationEventType::REDUCED_REDUNDANCY_LOST_OBJECT, "s3:ReducedRedundancyLostObject"},
            {NotificationEventType::OBJECT_CREATED, "s3:ObjectCreated"},
            {NotificationEventType::OBJECT_REMOVED, "s3:ObjectRemoved"},
            {NotificationEventType::OBJECT_RESTORED, "s3:ObjectRestore"},
            {NotificationEventType::REPLICATION, "s3:Replication"},
            {NotificationEventType::OBJECT_RESTORE, "s3:ObjectRestore"},
            {NotificationEventType::OBJECT_LIFECYCLE_TRANSITION, "s3:LifecycleTransition"},
            {NotificationEventType::INTELLIGENT_TIERING, "s3:IntelligentTiering"},
            {NotificationEventType::OBJECT_ACL, "s3:ObjectAcl"},
            {NotificationEventType::LIFECYCLE_EXPIRATION, "s3:LifecycleExpiration"},
            {NotificationEventType::OBJECT_TAGGING, "s3:ObjectTagging"},
    };

    [[maybe_unused]] static std::string EventTypeToString(const NotificationEventType eventType) {
        return EventTypeNames[eventType];
    }

    [[maybe_unused]] static NotificationEventType EventTypeFromString(const std::string &nameType) {
        for (auto &[fst, snd]: EventTypePrefixes) {
            if (nameType.starts_with(snd)) {
                return fst;
            }
        }
        return NotificationEventType::OBJECT_CREATED;
    }

    BOOST_DESCRIBE_ENUM(NotificationEventType,
                        REDUCED_REDUNDANCY_LOST_OBJECT,
                        OBJECT_CREATED,
                        OBJECT_REMOVED,
                        OBJECT_RESTORED,
                        REPLICATION,
                        OBJECT_RESTORE,
                        OBJECT_LIFECYCLE_TRANSITION,
                        INTELLIGENT_TIERING,
                        OBJECT_ACL,
                        LIFECYCLE_EXPIRATION,
                        OBJECT_TAGGING);

}// namespace Awsmock::Dto::S3
