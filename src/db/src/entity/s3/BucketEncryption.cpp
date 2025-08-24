//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/BucketEncryption.h>

namespace AwsMock::Database::Entity::S3 {
    view_or_value<view, value> BucketEncryption::ToDocument() const {
        try {
            view_or_value<view, value> notificationDoc = make_document(
                    kvp("sseAlgorithm", sseAlgorithm),
                    kvp("kmsKeyId", kmsKeyId));
            return notificationDoc;
        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void BucketEncryption::FromDocument(const view_or_value<view, value> &mResult) {
        try {
            sseAlgorithm = Core::Bson::BsonUtils::GetStringValue(mResult.view()["sseAlgorithm"]);
            kmsKeyId = Core::Bson::BsonUtils::GetStringValue(mResult.view()["kmsKeyId"]);
        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::S3
