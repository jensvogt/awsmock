//
// Created by vogje01 on 5/18/24.
//

#ifndef AWSMOCK_DTO_COMMON_BASE_COUNTER_H
#define AWSMOCK_DTO_COMMON_BASE_COUNTER_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/proto/traits.hpp>
#include <boost/version.hpp>

// AwsMock includes
#include "BaseClientCommand.h"


#include <awsmock/core/StringUtils.h>

namespace AwsMock::Dto::Common {
    class BaseClientCommand;

    /**
     * @brief Base Counter for the frontend
     *
     * @par
     * This base class declares generalized ToString and &lt;&lt; operator methods, as well as generalized ToJson, FromJson methods.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    template<typename T>
    struct BaseCounter {

        /**
         * Destructor
         */
        virtual ~BaseCounter() = default;

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
         * @brief Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] virtual std::string ToJson() const {
            std::stringstream ss;
            //ss << boost::json::value_from(*dynamic_cast<const BaseCounter *>(this));
            ss << boost::json::value_from(*dynamic_cast<const T *>(this));
            return ss.str();
        }

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        static T FromJson(const std::string &jsonString) {
            //boost::json::value_to<BaseCounter>(boost::json::parse(jsonString));
            return boost::json::value_to<T>(boost::json::parse(jsonString));
        }

        /**
         * @brief Convert from JSON representation base inside client command
         *
         * @param clientCommand base client command
         */
        static T FromJson(const BaseClientCommand &clientCommand) {
            T t = boost::json::value_to<T>(boost::json::parse(clientCommand.payload));
            t.region = clientCommand.region;
            t.user = clientCommand.user;
            t.requestId = clientCommand.requestId;
            return t;
        }

#ifndef _WIN32
        /**
         * @brief Return the demangled type name.
         *
         * @param name C++ mangled class name
         * @return demangled name
         */
        static std::string GetDemangledName(const std::string &name) {
            int status = -1;
            std::string demangledName = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
            return demangledName;
        }
#endif

        /**
         * @brief Generalized toString method
         *
         * @return JSON serialized object
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream s;
#ifdef _WIN32
            std::operator<<(s, typeid(T).name());
#else
            std::operator<<(s, GetDemangledName(typeid(T).name()));
#endif
            std::operator<<(s, std::string("="));
            std::operator<<(s, ToJson());
            return s.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        template<class S>
        friend std::ostream &operator<<(std::ostream &os, const BaseCounter &i);

      private:

        friend BaseCounter tag_invoke(boost::json::value_to_tag<BaseCounter>, boost::json::value const &v) {
            BaseCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, BaseCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
            };
        }
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_BASE_COUNTER_H
