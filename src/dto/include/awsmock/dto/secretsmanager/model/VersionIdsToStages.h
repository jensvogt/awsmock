//
// Created by vogje01 on 4/8/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H
#define AWSMOCK_DTO_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H

// C++ standard includes
#include <map>
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets manager version ID and stages
     *
     * Example:
     * @code{.json}
     * "EXAMPLE1-90ab-cdef-fedc-ba987SECRET1": [
     *   "AWSPREVIOUS"
     * ],
     * "EXAMPLE2-90ab-cdef-fedc-ba987SECRET2": [
     *   "AWSCURRENT"
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct VersionIdsToStages final : Common::BaseCounter<VersionIdsToStages> {

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

                document rootDocument;
                if (!versions.empty()) {
                    for (const auto &[fst, snd]: versions) {
                        array stagesArray;
                        document versionDocument;
                        for (const auto &stage: snd) {
                            stagesArray.append(stage);
                        }
                        versionDocument.append(kvp(fst, stagesArray));
                    }
                }
                return rootDocument.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param jsonObject JSON object.
         */
        void FromDocument(const view_or_value<view, value> &jsonObject) {

            /* Todo:
            try {

                //Core::JsonUtils::GetJsonValueString("Region", jsonObject, region);
                //Core::JsonUtils::GetJsonValueString("ARN", jsonObject, arn);

            } catch (Poco::Exception &exc) {
                std::cerr << exc.message() << std::endl;
                throw Core::ServiceException(exc.message());
            }*/
        }

      private:

        friend VersionIdsToStages tag_invoke(boost::json::value_to_tag<VersionIdsToStages>, boost::json::value const &v) {
            VersionIdsToStages r;
            r.versions = boost::json::value_to<std::map<std::string, std::vector<std::string>>>(v.at("Versions"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, VersionIdsToStages const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Version", boost::json::value_from(obj.versions)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H
