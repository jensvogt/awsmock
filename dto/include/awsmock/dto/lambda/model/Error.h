//
// Created by vogje01 on 4/30/24.
//

#ifndef AWSMOCK_DTO_LAMBDA_ERROR_H
#define AWSMOCK_DTO_LAMBDA_ERROR_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct Error final : Common::BaseCounter<Error> {

        /**
         * Error code
         */
        std::string errorCode;

        /**
         * Error message
         */
        std::string message;

      private:

        friend Error tag_invoke(boost::json::value_to_tag<Error>, boost::json::value const &v) {
            Error r;
            r.errorCode = Core::Json::GetStringValue(v, "errorCode");
            r.message = Core::Json::GetStringValue(v, "message");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Error const &obj) {
            jv = {
                    {"errorCode", obj.errorCode},
                    {"message", obj.message},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_ERROR_H
