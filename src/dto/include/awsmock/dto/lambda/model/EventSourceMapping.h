//
// Created by vogje01 on 7/13/24.
//

#ifndef AWSMOCK_DTO_LAMBDA_EVENT_SOURCE_MAPPING_H
#define AWSMOCK_DTO_LAMBDA_EVENT_SOURCE_MAPPING_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct EventSourceMapping final : Common::BaseCounter<EventSourceMapping> {

        /**
         * Type
         */
        std::string type;

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

        /**
         * Batch size
         */
        long batchSize{};

        /**
         * Maximal batches in windows
         */
        long maximumBatchingWindowInSeconds{};

        /**
         * UUID
         */
        std::string uuid;

        /**
         * @brief Creates a JSON object.
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Type", type);
                Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
                Core::Bson::BsonUtils::SetStringValue(document, "EventSourceArn", eventSourceArn);
                Core::Bson::BsonUtils::SetLongValue(document, "BatchSize", batchSize);
                Core::Bson::BsonUtils::SetLongValue(document, "MaximumBatchingWindowInSeconds", maximumBatchingWindowInSeconds);
                Core::Bson::BsonUtils::SetStringValue(document, "UUID", uuid);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend EventSourceMapping tag_invoke(boost::json::value_to_tag<EventSourceMapping>, boost::json::value const &v) {
            EventSourceMapping r;
            r.type = Core::Json::GetStringValue(v, "Type");
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.eventSourceArn = Core::Json::GetStringValue(v, "EventSourceArn");
            r.batchSize = Core::Json::GetLongValue(v, "BatchSize");
            r.maximumBatchingWindowInSeconds = Core::Json::GetLongValue(v, "MaximumBatchingWindowInSeconds");
            r.uuid = Core::Json::GetStringValue(v, "UUID");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EventSourceMapping const &obj) {
            jv = {
                    {"Type", obj.type},
                    {"FunctionArn", obj.functionArn},
                    {"EventSourceArn", obj.eventSourceArn},
                    {"BatchSize", obj.batchSize},
                    {"MaximumBatchingWindowInSeconds", obj.maximumBatchingWindowInSeconds},
                    {"UUID", obj.uuid},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_EVENT_SOURCE_MAPPING_H
