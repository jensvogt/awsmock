//
// Created by vogje01 on 12/21/23.
//

#ifndef AWSMOCK_ENTITY_DYNAMODB_STREAM_SPECIFICATION_H
#define AWSMOCK_ENTITY_DYNAMODB_STREAM_SPECIFICATION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/dynamodb/StreamViewType.h>

namespace AwsMock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB stream specification
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StreamSpecification final : Common::BaseEntity<StreamSpecification> {

        /**
         * Enabled
         */
        bool enabled = false;

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
            enabled = Core::Bson::BsonUtils::GetBoolValue(jsonObject, "streamEnabled");
            streamViewType = StreamViewTypeFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "streamViewType"));
        }

        /**
         * @brief Convert to JSON value
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            bsoncxx::builder::basic::document rootDocument;
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "streamEnabled", enabled);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "streamViewType", StreamViewTypeToString(streamViewType));
            return rootDocument.extract();
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
                    {"StreamEnabled", StreamViewTypeToString(obj.streamViewType)},

            };
        }
    };

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_ENTITY_DYNAMODB_STREAM_SPECIFICATION_H
