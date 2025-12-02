//
// Created by vogje01 on 4/30/24.
//

#ifndef AWSMOCK_DTO_LAMBDA_TAGS_H
#define AWSMOCK_DTO_LAMBDA_TAGS_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Lambda tags
     *
     * @author jens.vogt\@opitz--consulting.com
     */
    struct Tags final : Common::BaseCounter<Tags> {

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Checks whether a tag with the given tags key exists.
         *
         * @param key key of the tags
         * @return true if tag with the given key exists.
         */
        bool HasTag(const std::string &key) {
            return std::ranges::find_if(tags, [key](const std::pair<std::string, std::string> &t) {
                       return t.first == key;
                   }) != tags.end();
        }

        /**
         * Returns a given tags value by key
         *
         * @param key key of the tags
         * @return tag value
         */
        std::string GetTagValue(const std::string &key) {
            const auto it = std::ranges::find_if(tags, [key](const std::pair<std::string, std::string> &t) {
                return t.first == key;
            });
            return it->second;
        }

        /**
         * Convert to a JSON array
         *
         * @return JSON array
         */
        [[nodiscard]] array ToDocument() const {

            try {
                // Tags
                array jsonArray;
                if (!tags.empty()) {
                    for (const auto &[fst, snd]: tags) {
                        document tagObject;
                        tagObject.append(kvp(fst, snd));
                        jsonArray.append(tagObject);
                    }
                }
                return jsonArray;

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                if (document.view().find("Tags") != document.view().end()) {
                    for (const bsoncxx::array::view jsonArray = document.view()["Tags"].get_array().value; const auto &tag: jsonArray) {
                        std::string key = bsoncxx::string::to_string(tag.key());
                        const std::string value = bsoncxx::string::to_string(tag[key].get_string().value);
                        tags[key] = value;
                    }
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }


      private:

        friend Tags tag_invoke(boost::json::value_to_tag<Tags>, boost::json::value const &v) {
            Tags r;
            r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Tags const &obj) {
            jv = {
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_TAGS_H
