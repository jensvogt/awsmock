//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <map>
#include <string>

#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace Awsmock::Dto::KMS {

    struct ListResourceTagsResponse {

        std::map<std::string, std::string> tags;

        [[nodiscard]] std::string ToJson() const {
            try {
                // AWS returns Tags as [{TagKey, TagValue}] array
                document doc;
                auto tagsArr = bsoncxx::builder::basic::array{};
                for (const auto &[k, v]: tags) {
                    document tagDoc;
                    tagDoc.append(kvp("TagKey", k), kvp("TagValue", v));
                    tagsArr.append(tagDoc);
                }
                doc.append(kvp("Tags", tagsArr));
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
