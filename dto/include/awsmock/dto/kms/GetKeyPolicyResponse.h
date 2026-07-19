//
// Created by vogje01 on 19/07/2026.
//

#pragma once

// C++ includes
#include <string>

// Awsmock includes
#include <awsmock/core/BsonUtils.h>

namespace Awsmock::Dto::KMS {

    struct GetKeyPolicyResponse {

        /**
         * @brief KMS key policy.
         *
         * @par
         * Policy is a JSON-encoded string (as AWS returns it)
         */
        std::string policy;

        [[nodiscard]] std::string ToJson() const {
            try {
                document doc;
                Core::Bson::BsonUtils::SetStringValue(doc, "Policy", policy);
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
