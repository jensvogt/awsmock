//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/module/ExportInfrastructureRequest.h>

namespace AwsMock::Dto::Module {

    void ExportInfrastructureRequest::FromJson(const std::string &payload) {

        try {

            const value documentValue = bsoncxx::from_json(payload);
            includeObjects = Core::Bson::BsonUtils::GetBoolValue(documentValue, "includeObjects");
            prettyPrint = Core::Bson::BsonUtils::GetBoolValue(documentValue, "prettyPrint");
            cleanFirst = Core::Bson::BsonUtils::GetBoolValue(documentValue, "cleanFirst");

            if (documentValue.find("modules") != documentValue.end()) {
                Core::Bson::FromBsonStringArray(documentValue, "modules", &modules);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ExportInfrastructureRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "includeObjects", includeObjects);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "prettyPrint", prettyPrint);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "cleanFirst", cleanFirst);

            if (!modules.empty()) {
                array jsonArray;
                for (const auto &it: modules) {
                    jsonArray.append(it);
                }
                rootDocument.append(kvp("modules", jsonArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ExportInfrastructureRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ExportInfrastructureRequest &r) {
        os << "ExportInfrastructureRequest=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Module