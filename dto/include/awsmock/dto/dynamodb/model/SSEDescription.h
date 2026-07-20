//
// Created by vogje01 on 12/21/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>
#include <awsmock/dto/dynamodb/model/StreamViewType.h>

namespace Awsmock::Dto::DynamoDb {

    enum class SSEType {
        AES256,
        KMS
    };

    static std::map<SSEType, std::string> SSETypeNames{
            {SSEType::AES256, "AES256"},
            {SSEType::KMS, "KMS"},
    };

    [[maybe_unused]] static std::string SSETypeToString(const SSEType &sseType) {
        return SSETypeNames[sseType];
    }

    [[maybe_unused]] static SSEType SSETypeFromString(const std::string &sseType) {
        for (auto &[fst, snd]: SSETypeNames) {
            if (snd == sseType) {
                return fst;
            }
        }
        return SSEType::KMS;
    }

    enum class SSEStatusType {
        ENABLING,
        ENABLED,
        DISABLING,
        DISABLED,
        UPDATING
    };

    static std::map<SSEStatusType, std::string> SSEStatusTypeNames{
            {SSEStatusType::ENABLING, "ENABLING"},
            {SSEStatusType::ENABLED, "ENABLED"},
            {SSEStatusType::DISABLING, "DISABLING"},
            {SSEStatusType::DISABLED, "DISABLED"},
            {SSEStatusType::UPDATING, "UPDATING"},
    };

    [[maybe_unused]] static std::string SSEStatusTypeToString(const SSEStatusType &sseStatusType) {
        return SSEStatusTypeNames[sseStatusType];
    }

    [[maybe_unused]] static SSEStatusType SSEStatusTypeFromString(const std::string &sseStatusType) {
        for (auto &[fst, snd]: SSEStatusTypeNames) {
            if (snd == sseStatusType) {
                return fst;
            }
        }
        return SSEStatusType::DISABLED;
    }

    /**
     * @brief DynamoDB SSE description
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SSEDescription final : Common::BaseObject<SSEDescription> {

        /**
         * @brief Inaccessible encryption
         */
        int inaccessibleEncryptionDateTime{};

        /**
         * @brief KMS master key ARN
         */
        std::string kmsMasterKeyArn{};

        /**
         * @brief SSE type
         */
        SSEType sseType{};

        /**
         * @brief SSE type
         */
        SSEStatusType status{};

      private:

        friend SSEDescription tag_invoke(boost::json::value_to_tag<SSEDescription>, boost::json::value const &v) {
            SSEDescription r = {};
            r.inaccessibleEncryptionDateTime = Core::Json::GetIntValue(v, "InaccessibleEncryptionDateTime");
            r.kmsMasterKeyArn = Core::Json::GetStringValue(v, "KMSMasterKeyArn");
            r.sseType = SSETypeFromString(Core::Json::GetStringValue(v, "SSEType"));
            r.status = SSEStatusTypeFromString(Core::Json::GetStringValue(v, "Status"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SSEDescription const &obj) {
            jv = {
                    {"InaccessibleEncryptionDateTime", obj.inaccessibleEncryptionDateTime},
                    {"KMSMasterKeyArn", obj.kmsMasterKeyArn},
                    {"SSEType", SSETypeToString(obj.sseType)},
                    {"Status", SSEStatusTypeToString(obj.status)},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
