
#include <awsmock/dto/module/ListModuleNamesResponse.h>

namespace AwsMock::Dto::Module {

    std::string ListModuleNamesResponse::ToJson() const {

        try {

            document rootDocument;
            if (!moduleNames.empty()) {

                array jsonArray;
                for (auto &moduleName: moduleNames) {
                    jsonArray.append(moduleName);
                }
                rootDocument.append(kvp("modules", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ListModuleNamesResponse::FromJson(const std::string &jsonString) {

        try {

            if (const value document = bsoncxx::from_json(jsonString); document.view().find("modules") != document.view().end()) {
                for (const view jsonArray = document["modules"].get_array().value; const auto &moduleName: jsonArray) {
                    moduleNames.emplace_back(moduleName.get_string().value);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string ListModuleNamesResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListModuleNamesResponse &r) {
        os << "ListModuleNamesResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Module