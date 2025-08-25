//
// Created by JVO on 29.04.2024.
//

#ifndef AWSMOCK_DTO_S3_EVENT_H
#define AWSMOCK_DTO_S3_EVENT_H

// C++ standard includes
#include <string>

namespace AwsMock::Dto::S3 {

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
            {NotificationEventType::REDUCED_REDUNDANCY_LOST_OBJECT, "ReducedRedundancyLostObject "},
            {NotificationEventType::OBJECT_CREATED, "ObjectCreated"},
            {NotificationEventType::OBJECT_REMOVED, "ObjectRemoved"},
            {NotificationEventType::OBJECT_RESTORED, "ObjectRestore"},
            {NotificationEventType::REPLICATION, "Replication"},
            {NotificationEventType::OBJECT_RESTORE, "ObjectRestore"},
            {NotificationEventType::OBJECT_LIFECYCLE_TRANSITION, "LifecycleTransition "},
            {NotificationEventType::INTELLIGENT_TIERING, "IntelligentTiering "},
            {NotificationEventType::OBJECT_ACL, "ObjectAcl"},
            {NotificationEventType::LIFECYCLE_EXPIRATION, "LifecycleExpiration"},
            {NotificationEventType::OBJECT_TAGGING, "ObjectTagging"},
    };

    [[maybe_unused]] static std::string EventTypeToString(const NotificationEventType eventType) {
        return EventTypeNames[eventType];
    }

    [[maybe_unused]] static NotificationEventType EventTypeFromString(const std::string &nameType) {
        for (auto &[fst, snd]: EventTypeNames) {
            if (snd == nameType) {
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

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_EVENT_H
