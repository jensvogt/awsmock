//
// Created by vogje01 on 5/18/24.
//

#ifndef AWSMOCK_DTO_COMMON_BASEREQUEST_H
#define AWSMOCK_DTO_COMMON_BASEREQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>

namespace traits {
    template<typename T>
    concept has_ToString_v = requires(T &&t) { t->ToString(); };

    template<typename T>
    concept has_ostream_op_v = requires(T &&t, std::ostream &os) { os << t; };

    template<typename T>
    concept has_ToJson_op_v = requires(T &&t) {
        t->ToJson();
    };
}// namespace traits

namespace AwsMock::Dto::Common {

    /**
     * @brief Base request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    template<typename T>
    struct BaseRequest {

        /**
         * Destructor
         */
        virtual ~BaseRequest() = default;

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
         * @brief Generalized toString method
         *
         * @return JSON serialized object
         */
        std::string ToString();

        /**
         * @brief Virtual JSON conversion
         *
         * @return JSON serialized object
         */
        virtual std::string ToJson() = 0;

        /**
         * Typed member
         */
        T *t;
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_BASEREQUEST_H
