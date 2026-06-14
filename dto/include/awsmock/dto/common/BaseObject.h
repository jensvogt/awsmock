//
// Created by vogje01 on 5/18/24.
//

#pragma once

// C++ standard includes
#include <sstream>
#include <string>
#include <typeinfo>

#ifndef _WIN32
#include <cxxabi.h>
#endif

// Boost includes
#include <boost/json.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>

namespace Awsmock::Dto::Common {

    /**
     * @brief Base object for child DTO objects
     *
     * @par
     * Provides ToJson/FromJson, ToString and stream operator for child DTOs.
     * Does not carry request-level metadata (region, requestId, user).
     * Top-level request/response DTOs should use BaseCounter instead.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    template<typename T>
    struct BaseObject {

        virtual ~BaseObject() = default;

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
         * @return object of class <T>
         */
        static T FromJson(const std::string &jsonString) {
            if (jsonString.empty()) {
                return {};
            }
            const boost::json::value jv = boost::json::parse(jsonString);
            return boost::json::value_to<T>(jv);
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
            if (char *demangled = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status); status == 0 && demangled != nullptr) {
                std::string result(demangled);
                std::free(demangled);
                return result;
            }
            return name;
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
        friend std::ostream &operator<<(std::ostream &os, const BaseObject &e) {
            std::operator<<(os, e.ToString());
            return os;
        }
    };

}// namespace Awsmock::Dto::Common
