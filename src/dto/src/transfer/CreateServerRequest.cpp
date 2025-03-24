//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/CreateServerRequest.h>

namespace AwsMock::Dto::Transfer {

    std::string CreateServerRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Domain", domain);

            // Protocols
            if (!protocols.empty()) {
                array jsonProtocolArray;
                for (const auto protocol: protocols) {
                    jsonProtocolArray.append(ProtocolTypeToString(protocol));
                }
                rootDocument.append(kvp("Protocols", jsonProtocolArray));
            }

            // Tags
            if (!tags.empty()) {
                array jsonTagsArray;
                for (const auto [fst, snd]: tags) {
                    document tagDocument;
                    tagDocument.append(kvp(fst, snd));
                    jsonTagsArray.append(tagDocument);
                }
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }


    void CreateServerRequest::FromJson(const std::string &jsonString) {

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            domain = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Domain");

            // Protocols
            if (rootDocument.find("Protocols") != rootDocument.end()) {
                for (const view protocolsArray = rootDocument.view()["Protocols"].get_array().value; const auto &protocol: protocolsArray) {
                    protocols.emplace_back(ProtocolTypeFromString(protocol.get_string().value.data()));
                }
            }

            // Tags
            if (rootDocument.find("Tags") != rootDocument.end()) {
                for (const view tagsObject = rootDocument.view()["Tags"].get_document().value; const auto &tag: tagsObject) {
                    std::string key = bsoncxx::string::to_string(tag.key());
                    const std::string value = bsoncxx::string::to_string(tag[key].get_string().value);
                    tags[key] = value;
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateServerRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateServerRequest &r) {
        os << "CreateServerRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
