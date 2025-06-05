//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_KMS_MASTER_KEY_STATE_H
#define AWSMOCK_DTO_KMS_MASTER_KEY_STATE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS key state
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class KeyState {
        CREATING,
        ENABLED,
        DISABLED,
        PENDING_DELETION,
        PENDING_IMPORT,
        PENDING_REPLICATION_DELETION,
        UNAVAILABLE,
        UPDATING
    };

    static std::map<KeyState, std::string> KeyStateNames{
            {KeyState::CREATING, "Creating"},
            {KeyState::ENABLED, "Enabled"},
            {KeyState::DISABLED, "Disabled"},
            {KeyState::PENDING_DELETION, "PendingDeletion"},
            {KeyState::PENDING_IMPORT, "PendingImport"},
            {KeyState::PENDING_REPLICATION_DELETION, "PendingReplicaDeletion"},
            {KeyState::UNAVAILABLE, "Unavailable"},
            {KeyState::UPDATING, "Updating"}};

    [[maybe_unused]] static std::string KeyStateToString(const KeyState &keyState) {
        return KeyStateNames[keyState];
    }

    [[maybe_unused]] static KeyState KeyStateFromString(const std::string &keyState) {
        for (auto &[fst, snd]: KeyStateNames) {
            if (snd == keyState) {
                return fst;
            }
        }
        return KeyState::UNAVAILABLE;
    }

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_MASTER_KEY_STATE_H
