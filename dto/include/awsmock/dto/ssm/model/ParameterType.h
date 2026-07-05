//
// Created by vogje01 on 8/22/24.
//

#pragma once

// C++ standard includes
#include <map>
#include <sstream>
#include <string>

namespace Awsmock::Dto::SSM {

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
        for (auto &[fst, snd]: ParameterTypeNames) {
            if (snd == parameterType) {
                return fst;
            }
        }
        return ParameterType::string;
    }

    inline std::ostream &operator<<(std::ostream &os, ParameterType t) {
        return os << ParameterTypeToString(t);
    }
}// namespace Awsmock::Dto::SSM
