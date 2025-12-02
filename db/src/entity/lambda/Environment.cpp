//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Environment.h>

namespace AwsMock::Database::Entity::Lambda {

    view_or_value<view, value> Environment::ToDocument() const {

        try {
            document rootDocument;
            if (!variables.empty()) {
                document jsonObject;
                for (const auto &[fst, snd]: variables) {
                    jsonObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("variables", jsonObject));
            }
            return rootDocument.extract();

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    void Environment::FromDocument(const view_or_value<view, value> &mResult) {

        if (mResult.view().find("variables") != mResult.view().end()) {
            for (const view variablesView = mResult.view()["variables"].get_document().value; const bsoncxx::document::element &variableElement: variablesView) {
                std::string key = bsoncxx::string::to_string(variableElement.key());
                std::string value = bsoncxx::string::to_string(variablesView[key].get_string().value);
                variables.emplace(key, value);
            }
        }
    }
}// namespace AwsMock::Database::Entity::Lambda