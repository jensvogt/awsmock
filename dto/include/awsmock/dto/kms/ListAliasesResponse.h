//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>
#include <vector>

#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace Awsmock::Dto::KMS {

    struct AliasEntry {
        std::string aliasName;
        std::string aliasArn;
        std::string targetKeyId;
    };

    struct ListAliasesResponse {

        std::vector<AliasEntry> aliases;

        [[nodiscard]] std::string ToJson() const {
            try {
                document doc;
                auto arr = bsoncxx::builder::basic::array{};
                for (const auto &a: aliases) {
                    document aliasDoc;
                    aliasDoc.append(
                            kvp("AliasName", a.aliasName),
                            kvp("AliasArn", a.aliasArn),
                            kvp("TargetKeyId", a.targetKeyId));
                    arr.append(aliasDoc);
                }
                doc.append(kvp("Aliases", arr));
                doc.append(kvp("Truncated", false));
                return Core::Bson::BsonUtils::ToJsonString(doc);
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:
        mutable logger_t _logger{boost::log::keywords::channel = "KMS"};
    };

}// namespace Awsmock::Dto::KMS
