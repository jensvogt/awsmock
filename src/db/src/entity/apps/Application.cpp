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
        applicationDocument.append(kvp("type", type));
        applicationDocument.append(kvp("privatePort", bsoncxx::types::b_int64(privatePort)));
        applicationDocument.append(kvp("publicPort", bsoncxx::types::b_int64(publicPort)));
        applicationDocument.append(kvp("archive", archive));
        applicationDocument.append(kvp("version", version));
        applicationDocument.append(kvp("containerId", containerId));
        applicationDocument.append(kvp("containerName", containerName));
        applicationDocument.append(kvp("enabled", enabled));
        applicationDocument.append(kvp("status", status));
        applicationDocument.append(kvp("imageId", imageId));
        applicationDocument.append(kvp("imageSize", bsoncxx::types::b_int64(imageSize)));
        applicationDocument.append(kvp("imageMd5", imageMd5));
        applicationDocument.append(kvp("imageName", imageName));
        applicationDocument.append(kvp("description", description));
        applicationDocument.append(kvp("lastStarted", bsoncxx::types::b_date(lastStarted)));
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

        // Dependencies
        if (!dependencies.empty()) {
            array dependenciesArray;
            for (const auto &dependency: dependencies) {
                dependenciesArray.append(dependency);
            }
            applicationDocument.append(kvp("dependencies", dependenciesArray));
        }

        return applicationDocument.extract();
    }

    void Application::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        runtime = Core::Bson::BsonUtils::GetStringValue(mResult, "runtime");
        type = Core::Bson::BsonUtils::GetStringValue(mResult, "type");
        privatePort = Core::Bson::BsonUtils::GetLongValue(mResult, "privatePort");
        publicPort = Core::Bson::BsonUtils::GetLongValue(mResult, "publicPort");
        archive = Core::Bson::BsonUtils::GetStringValue(mResult, "archive");
        version = Core::Bson::BsonUtils::GetStringValue(mResult, "version");
        containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
        containerName = Core::Bson::BsonUtils::GetStringValue(mResult, "containerName");
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        enabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "enabled");
        imageId = Core::Bson::BsonUtils::GetStringValue(mResult, "imageId");
        imageSize = Core::Bson::BsonUtils::GetLongValue(mResult, "imageSize");
        imageMd5 = Core::Bson::BsonUtils::GetStringValue(mResult, "imageMd5");
        imageName = Core::Bson::BsonUtils::GetStringValue(mResult, "imageName");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        lastStarted = Core::Bson::BsonUtils::GetDateValue(mResult, "lastStarted");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Environment
        if (mResult.value().find("environment") != mResult.value().end()) {
            environment.clear();
            for (const view environmentObject = mResult.value()["environment"].get_document().value; const auto &e: environmentObject) {
                const std::string key = bsoncxx::string::to_string(e.key());
                const std::string value = bsoncxx::string::to_string(environmentObject[key].get_string().value);
                environment[key] = value;
            }
        }

        // Tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsObject = mResult.value()["tags"].get_document().value; const auto &t: tagsObject) {
                const std::string key = bsoncxx::string::to_string(t.key());
                const std::string value = bsoncxx::string::to_string(tagsObject[key].get_string().value);
                tags[key] = value;
            }
        }

        // Dependencies
        if (mResult.value().find("dependencies") != mResult.value().end()) {
            dependencies.clear();
            for (const view dependenciesArray = mResult.value()["dependencies"].get_array().value; const auto &d: dependenciesArray) {
                dependencies.emplace_back(d.get_string().value);
            }
        }
    }

}// namespace AwsMock::Database::Entity::Apps