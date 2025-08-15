//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/cognito/Group.h>

namespace AwsMock::Database::Entity::Cognito {

    view_or_value<view, value> Group::ToDocument() const {

        view_or_value<view, value> userDocument = make_document(
                kvp("region", region),
                kvp("groupName", groupName),
                kvp("userPoolId", userPoolId),
                kvp("description", description),
                kvp("roleArn", roleArn),
                kvp("precedence", bsoncxx::types::b_int64(precedence)),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));
        return userDocument;
    }

    void Group::FromDocument(const std::optional<view> &mResult) {

        // Could be null, in case the group is part of a user
        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        groupName = Core::Bson::BsonUtils::GetStringValue(mResult, "groupName");
        userPoolId = Core::Bson::BsonUtils::GetStringValue(mResult, "userPoolId");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        roleArn = Core::Bson::BsonUtils::GetStringValue(mResult, "roleArn");
        precedence = Core::Bson::BsonUtils::GetLongValue(mResult, "precedence");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");
    }

}// namespace AwsMock::Database::Entity::Cognito