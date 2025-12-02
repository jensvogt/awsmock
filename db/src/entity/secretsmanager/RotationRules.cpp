//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/secretsmanager/RotationRules.h>

namespace AwsMock::Database::Entity::SecretsManager {

    view_or_value<view, value> RotationRules::ToDocument() const {

        auto rotationRulesDoc = document{};
        rotationRulesDoc.append(kvp("automaticallyAfterDays", static_cast<bsoncxx::types::b_int64>(automaticallyAfterDays)));
        rotationRulesDoc.append(kvp("duration", duration));
        rotationRulesDoc.append(kvp("scheduleExpression", scheduleExpression));

        return rotationRulesDoc.extract();
    }

    void RotationRules::FromDocument(const std::optional<view> &mResult) {

        automaticallyAfterDays = Core::Bson::BsonUtils::GetLongValue(mResult, "automaticallyAfterDays");
        duration = Core::Bson::BsonUtils::GetStringValue(mResult, "duration");
        scheduleExpression = Core::Bson::BsonUtils::GetStringValue(mResult, "scheduleExpression");
    }

    std::string RotationRules::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const RotationRules &s) {
        os << "RotationRules=" << to_json(s.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SecretsManager
