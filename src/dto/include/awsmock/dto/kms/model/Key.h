//
// Created by vogje01 on 4/26/24.
//

#ifndef AWSMOCK_DTO_KMS_KEY_H
#define AWSMOCK_DTO_KMS_KEY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>
#include <awsmock/dto/kms/model/KeySpec.h>
#include <awsmock/dto/kms/model/KeyState.h>
#include <awsmock/dto/kms/model/KeyUsage.h>
#include <awsmock/dto/kms/model/Origin.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS metadata
     *
     * Example:
     * @code{.json}
     * {
     *   "Arn": "string",
     *   "AWSAccountId": "string",
     *   "CloudHsmClusterId": "string",
     *   "CreationDate": number,
     *   "CustomerMasterKeySpec": "string",
     *   "CustomKeyStoreId": "string",
     *   "DeletionDate": number,
     *   "Description": "string",
     *   "Enabled": boolean,
     *   "EncryptionAlgorithms": [ "string" ],
     *   "ExpirationModel": "string",
     *   "KeyId": "string",
     *   "KeyManager": "string",
     *   "KeySpec": "string",
     *   "KeyState": "string",
     *   "KeyUsage": "string",
     *   "MacAlgorithms": [ "string" ],
     *   "MultiRegion": boolean,
     *   "MultiRegionConfiguration": {
     *     "MultiRegionKeyType": "string",
     *     "PrimaryKey": {
     *       "Arn": "string",
     *       "Region": "string"
     *     },
     *     "ReplicaKeys": [
     *       {
     *         "Arn": "string",
     *         "Region": "string"
     *       }
     *     ]
     *   },
     *   "Origin": "string",
     *   "PendingDeletionWindowInDays": number,
     *   "SigningAlgorithms": [ "string" ],
     *   "ValidTo": number,
     *   "XksKeyConfiguration": {
     *     "Id": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Key final : Common::BaseCounter<Key> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Arn
         */
        std::string arn;

        /**
         * Key specification
         */
        KeySpec keySpec = KeySpec::SYMMETRIC_DEFAULT;

        /**
         * Key usage
         */
        KeyUsage keyUsage = KeyUsage::ENCRYPT_DECRYPT;

        /**
         * Key state
         */
        KeyState keyState = KeyState::UNAVAILABLE;

        /**
         * Multi region
         */
        bool multiRegion = false;

        /**
         * Description
         */
        std::string description;

        /**
         * Creation date
         */
        long creationDate = system_clock::to_time_t(system_clock::now());

        /**
         * Deletion date
         */
        long deletionDate = 0;

        /**
         * Enabled flag
         */
        bool enabled = true;

        /**
         * Origin
         */
        Origin origin = Origin::AWS_KMS;

        /**
         * Encryption algorithms
         */
        std::vector<EncryptionAlgorithm> encryptionAlgorithms;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
                Core::Bson::BsonUtils::SetStringValue(document, "KeySpec", KeySpecToString(keySpec));
                Core::Bson::BsonUtils::SetStringValue(document, "KeyUsage", KeyUsageToString(keyUsage));
                Core::Bson::BsonUtils::SetStringValue(document, "KeyState", KeyStateToString(keyState));
                Core::Bson::BsonUtils::SetStringValue(document, "Arn", arn);
                Core::Bson::BsonUtils::SetLongValue(document, "CreationDate", creationDate);
                Core::Bson::BsonUtils::SetLongValue(document, "DeletionDate", deletionDate);
                Core::Bson::BsonUtils::SetBoolValue(document, "MultiRegion", multiRegion);
                Core::Bson::BsonUtils::SetStringValue(document, "Origin", OriginToString(origin));

                if (!encryptionAlgorithms.empty()) {
                    array jsonArray;
                    for (const auto &element: encryptionAlgorithms) {
                        jsonArray.append(EncryptionAlgorithmsToString(element));
                    }
                    document.append(kvp("EncryptionAlgorithms", jsonArray));
                }
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to from a JSON object
         *
         * @param jsonObject JSON object
         */
        void FromDocument(const view_or_value<view, value> &jsonObject) {

            try {

                keyId = Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyId");
                keySpec = KeySpecFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeySpec"));
                keyUsage = KeyUsageFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyUsage"));
                keyState = KeyStateFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyState"));
                description = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Description");
                arn = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Arn");
                creationDate = Core::Bson::BsonUtils::GetLongValue(jsonObject, "CreationDate");
                multiRegion = Core::Bson::BsonUtils::GetBoolValue(jsonObject, "MultiRegion");
                enabled = Core::Bson::BsonUtils::GetBoolValue(jsonObject, "Enabled");
                origin = OriginFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "Origin"));

                // Grant tokens
                if (jsonObject.view().find("EncryptionAlgorithms") != jsonObject.view().end()) {
                    for (const bsoncxx::array::view jsonArray = jsonObject.view()["EncryptionAlgorithms"].get_array().value; const auto &element: jsonArray) {
                        encryptionAlgorithms.emplace_back(EncryptionAlgorithmsFromString(std::string(element.get_string().value)));
                    }
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }


      private:

        friend Key tag_invoke(boost::json::value_to_tag<Key>, boost::json::value const &v) {
            Key r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.arn = Core::Json::GetStringValue(v, "KeyArn");
            r.keySpec = KeySpecFromString(Core::Json::GetStringValue(v, "KeySpec"));
            r.keyUsage = KeyUsageFromString(Core::Json::GetStringValue(v, "KeyUsage"));
            r.keyState = KeyStateFromString(Core::Json::GetStringValue(v, "KeyState"));
            r.multiRegion = Core::Json::GetBoolValue(v, "MultiRegion");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.creationDate = Core::Json::GetLongValue(v, "CreationDate");
            r.deletionDate = Core::Json::GetLongValue(v, "DeletionDate");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            r.origin = OriginFromString(Core::Json::GetStringValue(v, "Origin"));
            if (Core::Json::AttributeExists(v, "EncryptionAlgorithms")) {
                for (boost::json::array encryptionAlgorithmArray = v.at("EncryptionAlgorithms").as_array(); const auto &a: encryptionAlgorithmArray) {
                    r.encryptionAlgorithms.emplace_back(EncryptionAlgorithmsFromString(a.get_string().data()));
                }
            }

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Key const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"KeyArn", obj.arn},
                    {"KeySpec", KeySpecToString(obj.keySpec)},
                    {"KeyUsage", KeyUsageToString(obj.keyUsage)},
                    {"KeyState", KeyStateToString(obj.keyState)},
                    {"MultiRegion", obj.multiRegion},
                    {"Description", obj.description},
                    {"CreationDate", obj.creationDate},
                    {"DeletionDate", obj.deletionDate},
                    {"Enabled", obj.enabled},
                    {"Origin", OriginToString(obj.origin)},
            };
            if (!obj.encryptionAlgorithms.empty()) {
                for (const auto &a: obj.encryptionAlgorithms) {
                    jv.as_string() = EncryptionAlgorithmsToString(a);
                }
            }
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_KEY_H
