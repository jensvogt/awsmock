//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_USER_CONTEXT_DATA_H
#define AWSMOCK_DTO_COGNITO_MODEL_USER_CONTEXT_DATA_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user context data
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserContextData final : Common::BaseCounter<UserContextData> {

        /**
         * Encoded data
         */
        std::string encodedData;

        /**
         * IP address
         */
        std::string ipAddress;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "EncodedData", encodedData);
                Core::Bson::BsonUtils::SetStringValue(document, "IpAddress", ipAddress);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend UserContextData tag_invoke(boost::json::value_to_tag<UserContextData>, boost::json::value const &v) {
            UserContextData r;
            r.encodedData = Core::Json::GetStringValue(v, "EncodedData");
            r.ipAddress = Core::Json::GetStringValue(v, "IpAddress");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserContextData const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"EncodedData", obj.encodedData},
                    {"IpAddress", obj.ipAddress},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_USER_CONTEXT_DATA_H
