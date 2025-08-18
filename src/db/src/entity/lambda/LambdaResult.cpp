//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/LambdaResult.h>

namespace AwsMock::Database::Entity::Lambda {

    view_or_value<view, value> LambdaResult::ToDocument() const {

        document lambdaDocResult;
        Core::Bson::BsonUtils::SetOidValue(lambdaDocResult, "_id", oid);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "region", region);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "name", lambdaName);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "arn", lambdaArn);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "runtime", runtime);
        Core::Bson::BsonUtils::SetLongValue(lambdaDocResult, "duration", duration);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "instanceId", instanceId);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "containerId", containerId);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "requestBody", requestBody);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "responseBody", responseBody);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "logMessages", logMessages);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "statusCode", httpStatusCode);
        Core::Bson::BsonUtils::SetStringValue(lambdaDocResult, "lambdaStatus", LambdaInstanceStatusToString(lambdaStatus));
        Core::Bson::BsonUtils::SetDateValue(lambdaDocResult, "timestamp", timestamp);

        return lambdaDocResult.extract();
    }

    void LambdaResult::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        lambdaArn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        lambdaName = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        runtime = Core::Bson::BsonUtils::GetStringValue(mResult, "runtime");
        duration = Core::Bson::BsonUtils::GetLongValue(mResult, "duration");
        instanceId = Core::Bson::BsonUtils::GetStringValue(mResult, "instanceId");
        containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
        httpStatusCode = Core::Bson::BsonUtils::GetStringValue(mResult, "statusCode");
        requestBody = Core::Bson::BsonUtils::GetStringValue(mResult, "requestBody");
        responseBody = Core::Bson::BsonUtils::GetStringValue(mResult, "responseBody");
        logMessages = Core::Bson::BsonUtils::GetStringValue(mResult, "logMessages");
        lambdaStatus = LambdaInstanceStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "lambdaStatus"));
        timestamp = Core::Bson::BsonUtils::GetDateValue(mResult, "timestamp");
    }

}// namespace AwsMock::Database::Entity::Lambda