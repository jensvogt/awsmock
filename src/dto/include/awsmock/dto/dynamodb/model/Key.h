//
// Created by vogje01 on 5/22/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_KEY_H
#define AWSMOCK_DTO_DYNAMODB_KEY_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB item key
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Key final : Common::BaseCounter<Key> {

        /**
         * Name
         */
        std::map<std::string, AttributeValue> keys;

        /**
         * @brief Convert from JSON object.
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {
                for (bsoncxx::document::element ele: document.view()) {

                    std::string name(ele.key());
                    AttributeValue attributeValue;
                    attributeValue.FromDocument(ele.get_document());
                    keys[name] = attributeValue;
                }
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to JSON value
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                if (!keys.empty()) {
                    for (const auto &[fst, snd]: keys) {
                        document.append(kvp(fst, snd.ToDocument()));
                    }
                }
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Key tag_invoke(boost::json::value_to_tag<Key>, boost::json::value const &v) {
            Key r;
            if (Core::Json::AttributeExists(v, "keys")) {
                r.keys = boost::json::value_to<std::map<std::string, AttributeValue>>(v.at("keys"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Key const &obj) {
            jv = {
                    {"keys", boost::json::value_from(obj.keys)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif//AWSMOCK_DTO_DYNAMODB_KEY_H
