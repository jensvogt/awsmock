//
// Created by vogje01 on 7/6/24.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Sent when successful delete was processed.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteMessageBatchResultEntry final : Common::BaseObject<DeleteMessageBatchResultEntry> {


        /**
         * @brief Default constructor
         */
        explicit DeleteMessageBatchResultEntry() = default;

        /**
         * @brief Constructor
         *
         * @param id message ID
         */
        explicit DeleteMessageBatchResultEntry(const std::string &id) {
            this->id = id;
        }

        /**
         * Id
         */
        std::string id;

      private:

        friend DeleteMessageBatchResultEntry tag_invoke(boost::json::value_to_tag<DeleteMessageBatchResultEntry>, boost::json::value const &v) {
            DeleteMessageBatchResultEntry r;
            r.id = Core::Json::GetStringValue(v, "Id");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageBatchResultEntry const &obj) {
            jv = {
                    {"Id", obj.id},
            };
        }
    };
}// namespace Awsmock::Dto::SQS
