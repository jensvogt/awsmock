//
// Created by vogje01 on 4/8/24.
//

#ifndef AWSMOCK_DB_ENTITY_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H
#define AWSMOCK_DB_ENTITY_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::SecretsManager {

    /**
     * @brief Secrets manager version ID and stages
     *
     * Example:
     * @code{.json}
     * {
     *   "EXAMPLE1-90ab-cdef-fedc-ba987SECRET1": [
     *     "AWSPREVIOUS"
     *   ],
     *   "EXAMPLE2-90ab-cdef-fedc-ba987SECRET2": [
     *     "AWSCURRENT"
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct VersionIdsToStages {

        /**
         * Version/stages map
         */
        std::map<std::string, std::vector<std::string>> versions;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                bsoncxx::builder::basic::document rootDocument;
                if (!versions.empty()) {
                    for (const auto &[fst, snd]: versions) {
                        bsoncxx::builder::basic::array stagesArray;
                        for (const auto &stage: snd) {
                            stagesArray.append(stage);
                        }
                        rootDocument.append(kvp(fst, stagesArray));
                    }
                }
                return rootDocument.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to a DTO.
         *
         * @param jsonObject JSON object.
         */
        void FromDocument(const view_or_value<view, value> &jsonObject) {
            // Get rotation rules
            if (jsonObject.view().find("versionIdsToStages") != jsonObject.view().end()) {
                for (const view rotationObject = jsonObject.view()["versionIdsToStages"].get_document().value; const auto &element: rotationObject) {
                    for (auto &it: element.get_document().value) {
                        const auto key = std::string{it.key()};
                        std::vector<std::string> stages;
                        for (view versionArray = it.get_array().value; auto &e: versionArray) {
                            stages.push_back(bsoncxx::string::to_string(e.get_string().value));
                        }
                        versions[std::string{it.key()}] = stages;
                    }
                }
            }
        }
    };

}// namespace AwsMock::Database::Entity::SecretsManager

#endif// AWSMOCK_DB_ENTITY_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H
