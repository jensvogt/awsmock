//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/EphemeralStorage.h>

namespace AwsMock::Database::Entity::Lambda {

    void EphemeralStorage::FromDocument(std::optional<bsoncxx::document::view> mResult) {
        size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
    }

    view_or_value<view, value> EphemeralStorage::ToDocument() const {

        view_or_value<view, value> ephemeralStorageDocument = make_document(kvp("size", static_cast<bsoncxx::types::b_int64>(size)));
        return ephemeralStorageDocument;
    }

    std::string EphemeralStorage::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const EphemeralStorage &m) {
        os << "EphemeralStorage=" << bsoncxx::to_json(m.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Lambda