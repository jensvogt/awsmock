//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Tags.h>

namespace AwsMock::Database::Entity::Lambda {

    bool Tags::HasTag(const std::string &key) {
        return std::ranges::find_if(tags, [key](const std::pair<std::string, std::string> &t) {
                   return t.first == key;
               }) != tags.end();
    }

    std::string Tags::GetTagValue(const std::string &key) {
        const auto it = std::ranges::find_if(tags, [key](const std::pair<std::string, std::string> &t) {
            return t.first == key;
        });
        return it->second;
    }

    void Tags::FromDocument(const std::optional<view> &mResult) {

        std::vector<std::string> keys;
        std::transform(mResult->begin(), mResult->end(), std::back_inserter(keys), [](const bsoncxx::document::element &ele) {
            return bsoncxx::string::to_string(ele.key());
        });

        for (auto &it: keys) {
            tags[it] = Core::Bson::BsonUtils::GetStringValue(mResult, it);
        }
    }

    view_or_value<view, value> Tags::ToDocument() const {

        // Convert environment to document
        auto tagDoc = array{};
        for (const auto &[fst, snd]: tags) {
            tagDoc.append(make_document(kvp(fst, snd)));
        }
        return make_document(kvp("Tags", tagDoc));
    }

    [[nodiscard]] std::string Tags::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Tags &t) {
        os << "Tags=" << bsoncxx::to_json(t.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Lambda