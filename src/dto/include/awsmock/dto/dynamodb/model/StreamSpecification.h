//
// Created by vogje01 on 12/21/23.
//

#ifndef AWSMOCK_DTO_DYNAMODB_STREAM_SPECIFICATION_H
#define AWSMOCK_DTO_DYNAMODB_STREAM_SPECIFICATION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/StreamViewType.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB stream specification
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StreamSpecification final : Common::BaseCounter<StreamSpecification> {

        /**
         * Enabled
         */
        bool enabled{};

        /**
         * Value
         */
        StreamViewType streamViewType = StreamViewType::UNKNOWN;

        /**
         * @brief Convert from JSON object.
         *
         * @param jsonObject JSON object
         */
        void FromDocument(const view &jsonObject) {
            enabled = Core::Bson::BsonUtils::GetBoolValue(jsonObject, "enabled");
            streamViewType = StreamViewTypeFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "streamViewType"));
        }

        /**
         * @brief Convert to JSON value
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            bsoncxx::builder::basic::document document;
            Core::Bson::BsonUtils::SetBoolValue(document, "enabled", enabled);
            Core::Bson::BsonUtils::SetStringValue(document, "streamViewType", StreamViewTypeToString(streamViewType));
            return document.extract();
        }

      private:

        friend StreamSpecification tag_invoke(boost::json::value_to_tag<StreamSpecification>, boost::json::value const &v) {
            StreamSpecification r = {};
            r.enabled = Core::Json::GetBoolValue(v, "StreamEnabled");
            r.streamViewType = StreamViewTypeFromString(Core::Json::GetStringValue(v, "StreamViewType"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StreamSpecification const &obj) {
            jv = {
                    {"StreamEnabled", obj.enabled},
            };
            if (obj.enabled) {
                jv.as_object()["StreamViewType"] = StreamViewTypeToString(obj.streamViewType);
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_STREAM_SPECIFICATION_H
