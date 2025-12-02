//
// Created by vogje01 on 8/22/24.
//

#ifndef AWSMOCK_DTO_SSM_MODEL_PARAMETER_TYPE_H
#define AWSMOCK_DTO_SSM_MODEL_PARAMETER_TYPE_H

// C++ standard includes
#include <map>
#include <sstream>
#include <string>

namespace AwsMock::Dto::SSM {

    /**
     * @brief SSM parameter type
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ParameterType {
        string,
        stringList,
        secureString
    };

    static std::map<ParameterType, std::string> ParameterTypeNames{
            {ParameterType::string, "String"},
            {ParameterType::stringList, "StringList"},
            {ParameterType::secureString, "SecureString"}};

    [[maybe_unused]] static std::string ParameterTypeToString(ParameterType parameterType) {
        return ParameterTypeNames[parameterType];
    }

    [[maybe_unused]] static ParameterType ParameterTypeFromString(const std::string &parameterType) {
        for (auto &it: ParameterTypeNames) {
            if (it.second == parameterType) {
                return it.first;
            }
        }
        return ParameterType::string;
    }

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_MODEL_PARAMETER_TYPE_H
