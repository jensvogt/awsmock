//
// Created by vogje01 on 3/8/26.
//

#pragma once
// C++ includes
#include <map>
#include <string>

namespace Awsmock::Dto::Module {

    enum ExportType {
        INFRA_STRUCTURE,
        OBJECTS,
        BOTH
    };


    static std::map<ExportType, std::string> ExportTypeNames{
            {INFRA_STRUCTURE, "INFRA_STRUCTURE"},
            {OBJECTS, "OBJECTS"},
            {BOTH, "BOTH"},
    };

    [[maybe_unused]] static std::string ExportTypeToString(const ExportType &exportType) {
        return ExportTypeNames[exportType];
    }

    [[maybe_unused]] static ExportType ExportTypeFromString(const std::string &exportType) {
        for (auto &[fst, snd]: ExportTypeNames) {
            if (snd == exportType) {
                return fst;
            }
        }
        return INFRA_STRUCTURE;
    }
}// namespace Awsmock::Dto::Module

