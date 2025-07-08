//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apps/Application.h>

namespace AwsMock::Database::Entity::Apps {

    view_or_value<view, value> Application::ToDocument() const {

        document applicationDocument;
        applicationDocument.append(kvp("region", region));
        applicationDocument.append(kvp("name", name));
        applicationDocument.append(kvp("runtime", runtime));
        applicationDocument.append(kvp("archive", archive));
        applicationDocument.append(kvp("version", version));
        applicationDocument.append(kvp("containerId", containerId));
        applicationDocument.append(kvp("enabled", enabled));
        applicationDocument.append(kvp("status", status));
        applicationDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        applicationDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));

        // Environment
        if (!environment.empty()) {
            document environmentDoc;
            for (const auto &[fst, snd]: environment) {
                environmentDoc.append(kvp(fst, snd));
            }
            applicationDocument.append(kvp("environment", environmentDoc));
        }

        // Tags
        if (!tags.empty()) {
            document tagsDoc;
            for (const auto &[fst, snd]: tags) {
                tagsDoc.append(kvp(fst, snd));
            }
            applicationDocument.append(kvp("tags", tagsDoc));
        }

        // Options
        if (!options.empty()) {
            document optionsDoc;
            for (const auto &[fst, snd]: options) {
                optionsDoc.append(kvp(fst, snd));
            }
            applicationDocument.append(kvp("options", optionsDoc));
        }

        return applicationDocument.extract();
    }

    void Application::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        runtime = Core::Bson::BsonUtils::GetStringValue(mResult, "runtime");
        archive = Core::Bson::BsonUtils::GetStringValue(mResult, "archive");
        version = Core::Bson::BsonUtils::GetStringValue(mResult, "version");
        containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        enabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "enabled");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Environment
        if (mResult.value().find("environment") != mResult.value().end()) {
            for (const view environmentObject = mResult.value()["environment"].get_document().value; const auto &e: environmentObject) {
                const std::string key = bsoncxx::string::to_string(e.key());
                const std::string value = bsoncxx::string::to_string(environmentObject[key].get_string().value);
                environment[key] = value;
            }
        }

        // Tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            for (const view tagsObject = mResult.value()["tags"].get_document().value; const auto &t: tagsObject) {
                const std::string key = bsoncxx::string::to_string(t.key());
                const std::string value = bsoncxx::string::to_string(tagsObject[key].get_string().value);
                environment[key] = value;
            }
        }

        // Options
        if (mResult.value().find("options") != mResult.value().end()) {
            for (const view optionsObject = mResult.value()["options"].get_document().value; const auto &o: optionsObject) {
                const std::string key = bsoncxx::string::to_string(o.key());
                const std::string value = bsoncxx::string::to_string(optionsObject[key].get_string().value);
                environment[key] = value;
            }
        }
    }

    std::string Application::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Application::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Application &application) {
        os << "Application=" << application.ToJson();
        return os;
    }
}// namespace AwsMock::Database::Entity::Apps