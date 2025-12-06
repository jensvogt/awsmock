//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_ENTITY_DYNAMODB_STREAM_VIEW_TYPE_H
#define AWSMOCK_ENTITY_DYNAMODB_STREAM_VIEW_TYPE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Database::Entity::DynamoDb {

    /**
     * @brief Stream view type
     *
     * KEYS_ONLY - Only the key attributes of the modified item are written to the stream.
     * NEW_IMAGE - The entire item, as it appears after it was modified, is written to the stream.
     * OLD_IMAGE - The entire item, as it appeared before it was modified, is written to the stream.
     * NEW_AND_OLD_IMAGES - Both the new and the old item images of the item are written to the stream.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class StreamViewType {
        KEYS_ONLY,
        NEW_IMAGE,
        OLD_IMAGE,
        NEW_AND_OLD_IMAGES,
        UNKNOWN
    };

    static std::map<StreamViewType, std::string> StreamViewTypeNames{
            {StreamViewType::KEYS_ONLY, "KEYS_ONLY"},
            {StreamViewType::NEW_IMAGE, "NEW_IMAGE"},
            {StreamViewType::OLD_IMAGE, "OLD_IMAGE"},
            {StreamViewType::NEW_AND_OLD_IMAGES, "NEW_AND_OLD_IMAGES"},
            {StreamViewType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string StreamViewTypeToString(const StreamViewType &streamViewType) {
        return StreamViewTypeNames[streamViewType];
    }

    [[maybe_unused]] static StreamViewType StreamViewTypeFromString(const std::string &streamViewTypeName) {
        for (auto &[fst, snd]: StreamViewTypeNames) {
            if (snd == streamViewTypeName) {
                return fst;
            }
        }
        return StreamViewType::UNKNOWN;
    }

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_STREAM_VIEW_TYPE_H
