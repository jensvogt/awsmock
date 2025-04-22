//
// Created by vogje01 on 5/18/24.
//

#ifndef AWSMOCK_DTO_COMMON_BASEREQUEST_H
#define AWSMOCK_DTO_COMMON_BASEREQUEST_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/version.hpp>

// AwsMock includes
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Dto::Common {

    template<typename T>
    std::ostream &operator<<(std::ostream &os, const T &t) {
        int status = -1;
        const std::unique_ptr<char, decltype(&std::free)> demangledPtr(abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status), std::free);
        const std::string_view typeName = demangledPtr && status == 0 ? demangledPtr.get() : typeid(T).name();
        std::operator<<(os, typeName);
        std::operator<<(os, std::string("="));
        std::operator<<(os, t.ToJson());
        return os;
    }

    /**
     * @brief Base DTO
     *
     * @par
     * This base class declares generalized ToString and &lt;&lt; operator methods.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    template<typename T>
    struct BaseDto {

        /**
         * Destructor
         */
        virtual ~BaseDto() = default;

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
         * @brief Virtual JSON conversion
         *
         * @return JSON serialized object
         */
        [[nodiscard]] virtual std::string ToJson() const = 0;

        /**
         * @brief Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson2() const {
            std::stringstream ss;
            ss << boost::json::value_from(*dynamic_cast<const T *>(this));
            return ss.str();
        }

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

        /**
         * @brief Generalized toString method
         *
         * @return JSON serialized object
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream s;
            std::operator<<(s, GetDemangledName(typeid(T).name()));
            std::operator<<(s, std::string("="));
            std::operator<<(s, ToJson2());
            return s.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        template<class S>
        friend std::ostream &operator<<(std::ostream &os, const BaseDto &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_BASEREQUEST_H
