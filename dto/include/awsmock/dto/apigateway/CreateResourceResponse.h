//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    using std::chrono::system_clock;

    /**
     * @brief Create REST API resource response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateResourceResponse final : Common::BaseCounter<CreateResourceResponse> {

        /**
         * Resource ID
         */
        std::string id;

        /**
         * Parent ID
         */
        std::string parentId;

        /**
         * Path
         */
        std::string path;

        /**
         * Path part
         */
        std::string pathPart;

        /**
         * Full invoke URL for this resource
         */
        std::string url;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend CreateResourceResponse tag_invoke(boost::json::value_to_tag<CreateResourceResponse>, boost::json::value const &v) {
            CreateResourceResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.parentId = Core::Json::GetStringValue(v, "parentId");
            r.path = Core::Json::GetStringValue(v, "path");
            r.pathPart = Core::Json::GetStringValue(v, "pathPart");
            r.url = Core::Json::GetStringValue(v, "url");
            r.created = Core::Json::GetDatetimeValue(v, "createdDate");
            r.modified = Core::Json::GetDatetimeValue(v, "lastUpdatedDate");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateResourceResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"id", obj.id},
                    {"parentId", obj.parentId},
                    {"path", obj.path},
                    {"pathPart", obj.pathPart},
                    {"url", obj.url},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
