//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_PUT_PARAMETER_REQUEST_H
#define AWSMOCK_DTO_SSM_PUT_PARAMETER_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/ssm/model/ParameterType.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Put parameter request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutParameterRequest final : Common::BaseCounter<PutParameterRequest> {

        /**
         * Parameter name
         */
        std::string name;

        /**
         * Parameter value
         */
        std::string parameterValue;

        /**
         * Description
         */
        std::string description;

        /**
         * Parameter type
         */
        ParameterType type = ParameterType::string;

        /**
         * KMS key ID
         */
        std::string kmsKeyArn;

        /**
         * Parameter tier
         */
        std::string tier;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend PutParameterRequest tag_invoke(boost::json::value_to_tag<PutParameterRequest>, boost::json::value const &v) {
            PutParameterRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.parameterValue = Core::Json::GetStringValue(v, "Value");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.type = ParameterTypeFromString(Core::Json::GetStringValue(v, "Type"));
            r.kmsKeyArn = Core::Json::GetStringValue(v, "KeyId");
            r.tier = Core::Json::GetStringValue(v, "Tier");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutParameterRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"Value", obj.parameterValue},
                    {"Description", obj.description},
                    {"Type", ParameterTypeToString(obj.type)},
                    {"KeyId", obj.kmsKeyArn},
                    {"Tier", obj.tier},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_PUT_PARAMETER_REQUEST_H
