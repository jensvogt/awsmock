//
// Created by vogje01 on 6/7/26.
//

#pragma once

// C++ includes
#include <chrono>
#include <map>

// Awsmock includes
#include <awsmock/dto/apigateway/model/ResourceMethod.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::ApiGateway {

    using std::chrono::system_clock;

    /**
     * @brief API gateway rest entity
     *
     * @par
     * The rest entity defines a rest endpoint inside the API gateway.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Resource final : Common::BaseObject<Resource> {

        /**
         * @brief Resource ID
         */
        std::string id;

        /**
         * @brief Parent ID
         */
        std::string parentId;

        /**
         * @brief Resource path
         */
        std::string path;

        /**
         * @brief Path part
         */
        std::string pathPart;

        /**
         * @brief Full invoke URL for this resource
         */
        std::string url;

        /**
         * @brief Resource methods
         */
        std::map<std::string, ResourceMethod> resourceMethods;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

      private:

        friend Resource tag_invoke(boost::json::value_to_tag<Resource>, boost::json::value const &v) {
            Resource r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.parentId = Core::Json::GetStringValue(v, "parentId");
            r.path = Core::Json::GetStringValue(v, "path");
            r.pathPart = Core::Json::GetStringValue(v, "pathPart");
            r.url = Core::Json::GetStringValue(v, "url");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            if (Core::Json::AttributeExists(v, "resourceMethods")) {
                r.resourceMethods = boost::json::value_to<std::map<std::string, ResourceMethod>>(v.at("resourceMethods"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Resource const &obj) {
            jv = {
                    {"id", obj.id},
                    {"parentId", obj.parentId},
                    {"path", obj.path},
                    {"pathPart", obj.pathPart},
                    {"url", obj.url},
                    {"resourceMethods", boost::json::value_from(obj.resourceMethods)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };
}// namespace Awsmock::Dto::ApiGateway
