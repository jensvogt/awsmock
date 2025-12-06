//
// Created by vogje01 on 12/10/23.
//

#include <awsmock/entity/sqs/RedrivePolicy.h>

namespace AwsMock::Database::Entity::SQS {

    view_or_value<view, value> RedrivePolicy::ToDocument() const {

        document redrivePolicyDoc;
        Core::Bson::BsonUtils::SetStringValue(redrivePolicyDoc, "deadLetterTargetArn", deadLetterTargetArn);
        Core::Bson::BsonUtils::SetLongValue(redrivePolicyDoc, "maxReceiveCount", maxReceiveCount);

        return redrivePolicyDoc.extract();
    }

    void RedrivePolicy::FromDocument(const std::optional<view> &mResult) {

        deadLetterTargetArn = Core::Bson::BsonUtils::GetStringValue(mResult, "deadLetterTargetArn");
        maxReceiveCount = Core::Bson::BsonUtils::GetIntValue(mResult, "maxReceiveCount");
    }


}// namespace AwsMock::Database::Entity::SQS
