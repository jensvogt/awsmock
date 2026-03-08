//
// Created by vogje01 on 3/8/26.
//

#ifndef AWSMOCK_DTO_MODULE_EXPORT_TYPE_H
#define AWSMOCK_DTO_MODULE_EXPORT_TYPE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Module {

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
}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_MODULE_EXPORT_TYPE_H
