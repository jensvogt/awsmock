//
// Created by vogje01 on 2/11/25.
//

#include <awsmock/dto/docker/model/LogConfig.h>

namespace AwsMock::Dto::Docker {

    void LogConfig::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            FromDocument(document.view());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LogConfig::FromDocument(const view_or_value<view, value> &document) {

        try {

            type = Core::Bson::BsonUtils::GetStringValue(document, "Type");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Docker