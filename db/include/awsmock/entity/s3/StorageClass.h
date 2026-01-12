//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_STORAGE_CLASS_H
#define AWSMOCK_DB_ENTITY_S3_STORAGE_CLASS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief S3 storage class
     *
     * @par
     * If storage class does not exist in entity 'STANDARD' is assumed.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class StorageClass {
        STANDARD,
        STANDARD_IA,
        ONEZONE_IA,
        EXPRESS_ONEZONE,
        GLACIER,
        GLACIER_IR,
        DEEP_ARCHIVE,
        INTELLIGENT_TIERING,
        REDUCED_REDUNDANCY
    };

    static std::map<StorageClass, std::string> StorageClassNames{
            {StorageClass::STANDARD, "STANDARD"},
            {StorageClass::STANDARD_IA, "STANDARD_IA"},
            {StorageClass::ONEZONE_IA, "ONEZONE_IA"},
            {StorageClass::EXPRESS_ONEZONE, "EXPRESS_ONEZONE"},
            {StorageClass::GLACIER, "GLACIER"},
            {StorageClass::GLACIER_IR, "GLACIER_IR"},
            {StorageClass::DEEP_ARCHIVE, "DEEP_ARCHIVE"},
            {StorageClass::INTELLIGENT_TIERING, "INTELLIGENT_TIERING"},
            {StorageClass::REDUCED_REDUNDANCY, "REDUCED_REDUNDANCY"},
    };

    [[maybe_unused]] static std::string StorageClassToString(const StorageClass &storageClass) {
        return StorageClassNames[storageClass];
    }

    [[maybe_unused]] static StorageClass StorageClassFromString(const std::string &storageClass) {
        for (auto &[fst, snd]: StorageClassNames) {
            if (snd == storageClass) {
                return fst;
            }
        }
        return StorageClass::STANDARD;
    }
}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_STORAGE_CLASS_H
