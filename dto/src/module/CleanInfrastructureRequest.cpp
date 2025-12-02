//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/module/CleanInfrastructureRequest.h>

namespace AwsMock::Dto::Module {

    void CleanInfrastructureRequest::FromJson(const std::string &payload) {

        const value documentValue = bsoncxx::from_json(payload);

        try {

            onlyObjects = Core::Bson::BsonUtils::GetBoolValue(documentValue, "onlyObjects");

            if (documentValue.find("modules") != documentValue.end()) {
                Core::Bson::FromBsonStringArray(documentValue, "modules", &modules);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CleanInfrastructureRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "onlyObjects", onlyObjects);

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

    std::string CleanInfrastructureRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CleanInfrastructureRequest &r) {
        os << "CleanInfrastructureRequest=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Module