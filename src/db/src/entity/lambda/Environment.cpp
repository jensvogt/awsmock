//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Environment.h>

namespace AwsMock::Database::Entity::Lambda {


    /*void Environment::FromDocument(const view_or_value<view, value> &jsonObject) {

        try {
            if (jsonObject.view().find("variables") != jsonObject.view().end()) {
                for (auto [value] = jsonObject.view()["variables"].get_array(); auto &v: value) {
                    for (auto &it: v.get_document().value) {
                        variables[std::string{it.key()}] = bsoncxx::string::to_string(it.get_string().value);
                    }
                }
            }

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }*/

    view_or_value<view, value> Environment::ToDocument() const {

        try {
            document rootDocument;
            if (!variables.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: variables) {
                    document object;
                    object.append(kvp("name", fst));
                    object.append(kvp("value", snd));
                    jsonArray.append(object);
                }
                rootDocument.append(kvp("variables", jsonArray));
            }
            return rootDocument.extract();

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    void Environment::FromDocument(const view_or_value<view, value> &mResult) {

        if (mResult.view().find("variables") != mResult.view().end()) {
            auto value = mResult.view()["variables"].get_array().value;
            for (auto &v: value) {
                for (auto &it: v.get_document().value) {
                    variables[std::string{it.key()}] = bsoncxx::string::to_string(it.get_string().value);
                }
            }
        }
    }
}// namespace AwsMock::Database::Entity::Lambda