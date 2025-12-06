//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/CreateTagRequest.h>

namespace AwsMock::Dto::Lambda {

    void CreateTagRequest::FromJson(const std::string &jsonString) {

        try {

            if (const value document = bsoncxx::from_json(jsonString); document.view().find("Tags") != document.view().end()) {
                for (const bsoncxx::array::view jsonArray = document.view()["Tags"].get_array().value; const auto &tag: jsonArray) {
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

    std::string CreateTagRequest::ToJson() const {

        try {
            document rootDocument;

            if (!tags.empty()) {
                document jsonObject;
                for (const auto &[fst, snd]: tags) {
                    jsonObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("Tags", jsonObject));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateTagRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateTagRequest &r) {
        os << "CreateTagRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
