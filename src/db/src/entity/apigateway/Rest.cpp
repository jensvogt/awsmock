//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apigateway/RestApi.h>

namespace AwsMock::Database::Entity::ApiGateway {

    view_or_value<view, value> RestApi::ToDocument() const {

        document keyDocument;
        keyDocument.append(kvp("region", region));
        keyDocument.append(kvp("id", id));
        keyDocument.append(kvp("name", name));
        keyDocument.append(kvp("description", description));
        keyDocument.append(kvp("endpointUrl", endpointUrl));
        keyDocument.append(kvp("apiKeySource", apiKeySource));
        keyDocument.append(kvp("version", version));
        keyDocument.append(kvp("cloneFrom", cloneFrom));
        keyDocument.append(kvp("policy", policy));
        keyDocument.append(kvp("rootResourceId", rootResourceId));
        keyDocument.append(kvp("disableExecuteApiEndpoint", disableExecuteApiEndpoint));
        keyDocument.append(kvp("minimumCompressionSize", bsoncxx::types::b_int64(minimumCompressionSize)));
        keyDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        keyDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));

        // Tags
        if (!tags.empty()) {
            document tagsDoc;
            for (const auto &[fst, snd]: tags) {
                tagsDoc.append(kvp(fst, snd));
            }
            keyDocument.append(kvp("tags", tagsDoc));
        }

        // Binary media types
        if (!binaryMediaTypes.empty()) {
            array binaryMediaTypesArray;
            for (const auto &binaryMediaType: binaryMediaTypes) {
                binaryMediaTypesArray.append(binaryMediaType);
            }
            keyDocument.append(kvp("binaryMediaTypes", binaryMediaTypesArray));
        }

        // Warnings
        if (!warnings.empty()) {
            array warningsArray;
            for (const auto &warning: warnings) {
                warningsArray.append(warning);
            }
            keyDocument.append(kvp("warnings", warningsArray));
        }
        return keyDocument.extract();
    }

    void RestApi::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        id = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        endpointUrl = Core::Bson::BsonUtils::GetStringValue(mResult, "endpointUrl");
        apiKeySource = Core::Bson::BsonUtils::GetStringValue(mResult, "apiKeySource");
        version = Core::Bson::BsonUtils::GetStringValue(mResult, "version");
        cloneFrom = Core::Bson::BsonUtils::GetStringValue(mResult, "cloneFrom");
        policy = Core::Bson::BsonUtils::GetStringValue(mResult, "policy");
        rootResourceId = Core::Bson::BsonUtils::GetStringValue(mResult, "rootResourceId");
        disableExecuteApiEndpoint = Core::Bson::BsonUtils::GetBoolValue(mResult, "disableExecuteApiEndpoint");
        minimumCompressionSize = Core::Bson::BsonUtils::GetLongValue(mResult, "minimumCompressionSize");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsObject = mResult.value()["tags"].get_document().value; const auto &t: tagsObject) {
                const std::string key = bsoncxx::string::to_string(t.key());
                const std::string value = bsoncxx::string::to_string(tagsObject[key].get_string().value);
                tags[key] = value;
            }
        }

        // Binary media types
        if (mResult.value().find("binaryMediaTypes") != mResult.value().end()) {
            binaryMediaTypes.clear();
            for (const view binaryMediaTypeArray = mResult.value()["binaryMediaTypes"].get_array().value; const auto &b: binaryMediaTypeArray) {
                binaryMediaTypes.emplace_back(b.get_string().value);
            }
        }

        // Warnings
        if (mResult.value().find("warnings") != mResult.value().end()) {
            binaryMediaTypes.clear();
            for (const view binaryMediaTypeArray = mResult.value()["warnings"].get_array().value; const auto &b: binaryMediaTypeArray) {
                warnings.emplace_back(b.get_string().value);
            }
        }
    }

}// namespace AwsMock::Database::Entity::ApiGateway