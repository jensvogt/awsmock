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

      private:

        friend VersionIdsToStages tag_invoke(boost::json::value_to_tag<VersionIdsToStages>, boost::json::value const &v) {
            VersionIdsToStages r;
            r.versions = boost::json::value_to<std::map<std::string, std::vector<std::string>>>(v.at("Versions"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, VersionIdsToStages const &obj) {
            jv = {};
            for (const auto &[fst, snd]: obj.versions) {
                boost::json::array stagesArray;
                for (const auto &stage: snd) {
                    stagesArray.push_back(stage.c_str());
                }
                jv.as_object()[fst] = stagesArray;
            }
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_VERSION_IDS_TO_STAGES_H
