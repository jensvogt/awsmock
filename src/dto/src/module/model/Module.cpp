//
// Created by vogje01 on 10/23/23.
//

#include <awsmock/dto/module/model/Module.h>

namespace AwsMock::Dto::Module {

    std::string Module::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "name", name);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "state", ModuleStateToString(status));
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Module::ToJson(const Database::Entity::Module::ModuleList &moduleList) {

        try {

            document rootDocument;
            if (!moduleList.empty()) {
                array jsonArray;
                for (const auto &service: moduleList) {
                    document jsonElement;
                    Core::Bson::BsonUtils::SetStringValue(jsonElement, "name", service.name);
                    Core::Bson::BsonUtils::SetStringValue(jsonElement, "state", ModuleStateToString(service.state));
                    Core::Bson::BsonUtils::SetDateValue(jsonElement, "created", service.created);
                    Core::Bson::BsonUtils::SetDateValue(jsonElement, "modified", service.modified);
                    jsonArray.append(jsonElement.extract());
                }
                rootDocument.append(kvp("modules", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Module::ToJson(const ModuleList &moduleList) {

        try {

            document rootDocument;
            if (!moduleList.empty()) {
                array jsonArray;
                for (const auto &service: moduleList) {
                    document jsonElement;
                    Core::Bson::BsonUtils::SetStringValue(jsonElement, "name", service.name);
                    Core::Bson::BsonUtils::SetIntValue(jsonElement, "port", service.port);
                    Core::Bson::BsonUtils::SetStringValue(jsonElement, "state", Database::Entity::Module::ModuleStateToString(service.status));
                    Core::Bson::BsonUtils::SetDateValue(jsonElement, "created", service.created);
                    Core::Bson::BsonUtils::SetDateValue(jsonElement, "modified", service.modified);
                    jsonArray.append(jsonElement);
                }
                rootDocument.append(kvp("modules", jsonArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Module Module::FromJson(const std::string &payload) {

        if (payload.empty()) {
            return {};
        }

        Module module;

        try {

            const value documentValue = bsoncxx::from_json(payload);
            module.name = Core::Bson::BsonUtils::GetStringValue(documentValue, "name");
            module.port = Core::Bson::BsonUtils::GetIntValue(documentValue, "port");
            module.status = Database::Entity::Module::ModuleStateFromString(Core::Bson::BsonUtils::GetStringValue(documentValue, "state"));

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
        return module;
    }

    Module Module::FromDocument(const view &document) {

        Module module;

        try {

            module.name = Core::Bson::BsonUtils::GetStringValue(document, "name");
            module.port = Core::Bson::BsonUtils::GetIntValue(document, "port");
            module.status = Database::Entity::Module::ModuleStateFromString(Core::Bson::BsonUtils::GetStringValue(document, "state"));

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
        return module;
    }

    std::vector<Module> Module::FromJsonList(const std::string &payload) {

        if (payload.empty()) {
            return {};
        }
        std::vector<Module> modules;

        try {
            if (const value document = bsoncxx::from_json(payload); document.view().find("modules") != document.view().end()) {
                for (const auto &m: document.view()["modules"].get_array().value) {
                    Module module;
                    module = module.FromDocument(m.get_document().value);
                    modules.emplace_back(module);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
        return modules;
    }

    std::string Module::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Module &m) {
        os << "Module=" << m.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Module
