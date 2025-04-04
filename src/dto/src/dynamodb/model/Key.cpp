//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/model/AttributeValue.h>
#include <awsmock/dto/dynamodb/model/Key.h>

namespace AwsMock::Dto::DynamoDb {

    view_or_value<view, value> Key::ToDocument() const {

        try {

            document document;
            if (!keys.empty()) {
                for (const auto &[fst, snd]: keys) {
                    document.append(kvp(fst, snd.ToDocument()));
                }
            }
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void Key::FromDocument(const view_or_value<view, value> &document) {

        try {
            for (bsoncxx::document::element ele: document.view()) {

                std::string name(ele.key());
                AttributeValue attributeValue;
                attributeValue.FromDocument(ele.get_document());
                keys[name] = attributeValue;
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Key::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Key::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Key &r) {
        os << "Key=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::DynamoDb
