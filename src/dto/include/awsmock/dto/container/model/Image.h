//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_IMAGE_H
#define AWSMOCK_DTO_DOCKER_IMAGE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker image
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Image final : Common::BaseCounter<Image> {

        /**
         * Image ID
         */
        std::string id;

        /**
         * Parent ID
         */
        std::string parentId;

        /**
         * Repository digest
         */
        std::string repoDigest;

        /**
         * Repo tags
         */
        std::vector<std::string> repoTags;

        /**
         * Shared size
         */
        long sharedSize{};

        /**
         * Size
         */
        long size{};

        /**
         * Size
         */
        long virtualSize{};

        /**
         * Repo tags
         */
        std::map<std::string, std::string> labels;

        /**
         * Number of containers using this image
         */
        int containers{};

      private:

        friend Image tag_invoke(boost::json::value_to_tag<Image>, boost::json::value const &v) {
            Image r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.parentId = Core::Json::GetStringValue(v, "ParentId");
            r.repoDigest = Core::Json::GetStringValue(v, "RepoDigest");
            r.sharedSize = Core::Json::GetLongValue(v, "SharedSize");
            r.size = Core::Json::GetLongValue(v, "Size");
            r.virtualSize = Core::Json::GetLongValue(v, "VirtualSize");
            r.containers = Core::Json::GetIntValue(v, "Containers");
            if (Core::Json::AttributeExists(v, "RepoTags")) {
                r.repoTags = boost::json::value_to<std::vector<std::string>>(v.at("RepoTags"));
            }
            if (Core::Json::AttributeExists(v, "Labels")) {
                r.labels = boost::json::value_to<std::map<std::string, std::string>>(v.at("Labels"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Image const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"ParentId", obj.parentId},
                    {"RepoDigest", obj.repoDigest},
                    {"SharedSize", obj.sharedSize},
                    {"Size", obj.size},
                    {"VirtualSize", obj.virtualSize},
                    {"Containers", obj.containers},
                    {"RepoTags", boost::json::value_from(obj.repoTags)},
                    {"Labels", boost::json::value_from(obj.labels)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_IMAGE_H
