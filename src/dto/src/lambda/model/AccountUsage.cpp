//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/dto/lambda/model/AccountUsage.h>

namespace AwsMock::Dto::Lambda {

    view_or_value<view, value> AccountUsage::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetLongValue(document, "FunctionCount", functionCount);
            Core::Bson::BsonUtils::SetLongValue(document, "TotalCodeSize", totalCodeSize);
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string AccountUsage::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string AccountUsage::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const AccountUsage &f) {
        os << "AccountLimit=" << f.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Lambda
