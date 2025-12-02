//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_RESTORESTATUS_H
#define AWSMOCK_DTO_DYNAMODB_RESTORESTATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Table status
     *
     * CREATING - The table is being created.
     * UPDATING - The table/index configuration is being updated. The table/index remains available for data operations when UPDATING.
     * DELETING - The table is being deleted.
     * ACTIVE - The table is ready for use.
     * INACCESSIBLE_ENCRYPTION_CREDENTIALS - The AWS KMS key used to encrypt the table in inaccessible. Table operations may fail due to failure to use the AWS KMS key. DynamoDB will initiate the table archival process when a table's AWS KMS key remains inaccessible for more than seven days.
     * ARCHIVING - The table is being archived. Operations are not allowed until archival is complete.
     * ARCHIVED - The table has been archived. See the ArchivalReason for more information.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class TableStatusType {
        CREATING,
        UPDATING,
        DELETING,
        ACTIVE,
        INACCESSIBLE_ENCRYPTION_CREDENTIALS,
        ARCHIVING,
        ARCHIVED,
        UNKNOWN
    };

    static std::map<TableStatusType, std::string> TableStatusTypeNames{
            {TableStatusType::CREATING, "CREATING"},
            {TableStatusType::UPDATING, "UPDATING"},
            {TableStatusType::DELETING, "DELETING"},
            {TableStatusType::ACTIVE, "ACTIVE"},
            {TableStatusType::INACCESSIBLE_ENCRYPTION_CREDENTIALS, "INACCESSIBLE_ENCRYPTION_CREDENTIALS"},
            {TableStatusType::ARCHIVING, "ARCHIVING"},
            {TableStatusType::ARCHIVED, "ARCHIVED"},
            {TableStatusType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string TableStatusTypeToString(TableStatusType tableStatusType) {
        return TableStatusTypeNames[tableStatusType];
    }

    [[maybe_unused]] static TableStatusType TableStatusTypeFromString(const std::string &tableStatusName) {
        for (auto &[fst, snd]: TableStatusTypeNames) {
            if (snd == tableStatusName) {
                return fst;
            }
        }
        return TableStatusType::UNKNOWN;
    }

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_RESTORESTATUS_H
