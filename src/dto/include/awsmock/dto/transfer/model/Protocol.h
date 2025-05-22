//
// Created by JVO on 29.04.2024.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_PROTOCOL_H
#define AWSMOCK_DTO_TRANSFER_SERVER_PROTOCOL_H

// C++ standard includes
#include <string>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Transfer server protocol
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ProtocolType {
        FTP,
        SFTP,
        UNKNOWN
    };

    static std::map<ProtocolType, std::string> ProtocolTypeNames{
            {ProtocolType::FTP, "FTP"},
            {ProtocolType::SFTP, "SFTP"},
            {ProtocolType::UNKNOWN, "UNKNOWN"},
    };

    static std::string ProtocolTypeToString(const ProtocolType ProtocolType) {
        return ProtocolTypeNames[ProtocolType];
    }

    static ProtocolType ProtocolTypeFromString(const std::string &nameType) {
        for (auto &[fst, snd]: ProtocolTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(snd, nameType)) {
                return fst;
            }
        }
        return ProtocolType::UNKNOWN;
    }


}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_SERVER_PROTOCOL_H
