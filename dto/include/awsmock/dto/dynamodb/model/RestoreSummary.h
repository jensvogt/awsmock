//
// Created by vogje01 on 12/21/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB restore summary
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RestoreSummary final : Common::BaseObject<RestoreSummary> {

        /**
         * @brief Restore timestamp
         */
        std::int64_t restoreDateTime{};

        /**
         * @brief Restore in progress
         */
        bool restoreInProgress{false};

        /**
         * @brief Source backup ARN
         */
        std::string sourceBackupArn;

        /**
         * @brief Source table ARN
         */
        std::string sourceTableArn;

      private:

        friend RestoreSummary tag_invoke(boost::json::value_to_tag<RestoreSummary>, boost::json::value const &v) {
            RestoreSummary r = {};
            r.restoreDateTime = Core::Json::GetLongValue(v, "RestoreDateTime");
            r.restoreInProgress = Core::Json::GetBoolValue(v, "RestoreInProgress");
            r.sourceBackupArn = Core::Json::GetStringValue(v, "SourceBackupArn");
            r.sourceTableArn = Core::Json::GetStringValue(v, "SourceTableArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RestoreSummary const &obj) {
            jv = {
                    {"RestoreDateTime", obj.restoreDateTime},
                    {"RestoreInProgress", obj.restoreInProgress},
                    {"SourceBackupArn", obj.sourceBackupArn},
                    {"SourceTableArn", obj.sourceTableArn},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
