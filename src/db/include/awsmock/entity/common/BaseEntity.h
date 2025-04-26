//
// Created by vogje01 on 5/18/24.
//

#ifndef AWSMOCK_DB_ENTITY_COMMON_BASE_ENTITY_H
#define AWSMOCK_DB_ENTITY_COMMON_BASE_ENTITY_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/version.hpp>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <boost/proto/traits.hpp>

namespace AwsMock::Database::Entity::Common {

    /**
     * @brief Base entity
     *
     * @par
     * This base class declares generalized ToString and &lt;&lt; operator methods, as well as generalized ToJson, FromJson methods.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    template<typename T>
    struct BaseEntity {

        /**
         * Destructor
         */
        virtual ~BaseEntity() = default;

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
            ss << boost::json::value_from(*dynamic_cast<const T *>(this));
            return ss.str();
        }

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        static T FromJson(const std::string &jsonString) {
            return boost::json::value_to<T>(boost::json::parse(jsonString));
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
        friend std::ostream &operator<<(std::ostream &os, const BaseEntity &i);
    };

}// namespace AwsMock::Database::Entity::Common

#endif// AWSMOCK_DB_ENTITY_COMMON_BASE_ENTITY_H
