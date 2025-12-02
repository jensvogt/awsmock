//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/BucketNotification.h>

namespace AwsMock::Database::Entity::S3 {

    view_or_value<view, value> BucketNotification::ToDocument() const {

        view_or_value<view, value> notificationDoc = make_document(
                kvp("notificationId", notificationId),
                kvp("event", event),
                kvp("queueArn", queueArn),
                kvp("lambdaArn", lambdaArn));

        return notificationDoc;
    }

}// namespace AwsMock::Database::Entity::S3
