//
// Created by vogje01 on 5/18/24.
//

#pragma once

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::Common {

    class BaseClientCommand;

    /**
     * @brief Base counter for top-level request/response DTOs
     *
     * @par
     * Extends BaseObject with request-level metadata: region, requestId, user, contentType.
     * Also provides FromJson overloads that populate those fields from a client command.
     * Child DTO objects (model) should use BaseObject instead.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    template<typename T>
    struct BaseCounter : BaseObject<T> {

        /**
         * Request ID
         */
        std::string requestId = Core::StringUtils::CreateRandomUuid();

        /**
         * Region
         */
        std::string region{};

        /**
         * User
         */
        std::string user{};

        /**
         * Content type
         */
        std::string contentType{};

        using BaseObject<T>::FromJson;

        /**
         * @brief Serialize to JSON, automatically injecting region/user/requestId metadata.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const override {
            boost::json::value jv = boost::json::value_from(*dynamic_cast<const T *>(this));
            if (jv.is_object()) {
                auto &obj = jv.as_object();
                obj["region"] = region;
                obj["user"] = user;
                obj["requestId"] = requestId;
            }
            std::stringstream ss;
            ss << jv;
            return ss.str();
        }

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         * @param region
         * @param user
         * @param requestId
         * @return object of class <T>
         */
        static T FromJson(const std::string &jsonString, const std::string &region, const std::string &user, const std::string &requestId) {
            if (jsonString.empty()) {
                return {};
            }
            const boost::json::value jv = boost::json::parse(jsonString);
            T t = boost::json::value_to<T>(jv);
            t.region = region;
            t.user = user;
            t.requestId = requestId;
            return t;
        }

        /**
         * @brief Convert payload from client command in JSON format
         *
         * @param clientCommand base client command
         * @return object of class <T>
         */
        static T FromJson(const BaseClientCommand &clientCommand) {
            return BaseCounter::FromJson(clientCommand.payload, clientCommand.region, clientCommand.user, clientCommand.requestId);
        }

        /**
         * @brief Copy region, user, requestId from another BaseCounter into this object.
         *
         * @param source request DTO to copy metadata from
         */
        template<typename U>
        void copyMetadata(const BaseCounter<U> &source) {
            region = source.region;
            user = source.user;
            requestId = source.requestId;
        }

      private:

        /**
         * brief Deserialization
         *
         * @param v boost variant
         * @return
         */
        friend BaseCounter tag_invoke(boost::json::value_to_tag<BaseCounter>, boost::json::value const &v) {
            BaseCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            return r;
        }

        /**
         * brief Serialization
         *
         * @param jv boost json value
         * @param obj object to serialize
         */
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, BaseCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
            };
        }
    };

}// namespace Awsmock::Dto::Common
