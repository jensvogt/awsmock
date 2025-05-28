//
// Created by vogje01 on 5/23/25.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_VERSION_STAGE_H
#define AWSMOCK_DTO_SECRETSMANAGER_VERSION_STAGE_H

// C++ standard includes
#include <map>
#include <string>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief KMS key usage
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class VersionStateType {
        AWSCURRENT,
        AWSPENDING,
        AWSPREVIOUS
    };

    static std::map<VersionStateType, std::string> VersionStateNames{
            {VersionStateType::AWSCURRENT, "AWSCURRENT"},
            {VersionStateType::AWSPENDING, "AWSPENDING"},
            {VersionStateType::AWSPREVIOUS, "AWSPREVIOUS"}};

    [[maybe_unused]] static std::string VersionStateToString(const VersionStateType versionState) {
        return VersionStateNames[versionState];
    }

    [[maybe_unused]] static VersionStateType VersionStateFromString(const std::string &versionState) {
        for (auto &[fst, snd]: VersionStateNames) {
            if (snd == versionState) {
                return fst;
            }
        }
        return VersionStateType::AWSCURRENT;
    }

}// namespace AwsMock::Dto::SecretsManager


#endif//VERSIONSTAGE_H
