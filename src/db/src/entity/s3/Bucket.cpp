//
// Created by vogje01 on 03/09/2023.
//

#include <bsoncxx/json.hpp>
#include "awsmock/entity/s3/Bucket.h"

namespace AwsMock::Database::Entity::S3 {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;
  using bsoncxx::builder::basic::kvp;

  bool Bucket::HasNotification(const std::string &eventName) {
    return find_if(notifications.begin(), notifications.end(), [eventName](const BucketNotification &eventNotification) {
      return eventNotification.event == eventName;
    }) != notifications.end();
  }

  BucketNotification Bucket::GetNotification(const std::string &eventName) {
    auto it = find_if(notifications.begin(), notifications.end(), [eventName](const BucketNotification &eventNotification) {
      return eventNotification.event == eventName;
    });
    return *it;
  }

  bool Bucket::IsVersioned() const {
    return versionStatus == ENABLED;
  }

  view_or_value<view, value> Bucket::ToDocument() const {

    auto notificationsDoc = bsoncxx::builder::basic::array{};
    for (const auto &notification : notifications) {
      notificationsDoc.append(notification.ToDocument());
    }

    view_or_value<view, value> bucketDoc = make_document(
      kvp("region", region),
      kvp("name", name),
      kvp("owner", owner),
      kvp("notifications", notificationsDoc),
      kvp("versionStatus", BucketVersionStatusToString(versionStatus)),
      kvp("created", bsoncxx::types::b_date(std::chrono::milliseconds(created.timestamp().epochMicroseconds() / 1000))),
      kvp("modified", bsoncxx::types::b_date(std::chrono::milliseconds(modified.timestamp().epochMicroseconds() / 1000))));

    return bucketDoc;
  }

  void Bucket::FromDocument(mongocxx::stdx::optional<bsoncxx::document::value> mResult) {

    //bsoncxx::to_json(mResult, bsoncxx::ExtendedJsonMode::k_relaxed)

    oid = mResult.value()["_id"].get_oid().value.to_string();
    region = bsoncxx::string::to_string(mResult.value()["region"].get_string().value);
    name = bsoncxx::string::to_string(mResult.value()["name"].get_string().value);
    owner = bsoncxx::string::to_string(mResult.value()["owner"].get_string().value);
    versionStatus = BucketVersionStatusFromString(bsoncxx::string::to_string(mResult.value()["versionStatus"].get_string().value));
    created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
    modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

    bsoncxx::array::view notificationView{mResult.value()["notifications"].get_array().value};
    for (bsoncxx::array::element notificationElement : notificationView) {
      BucketNotification notification{
        .event=bsoncxx::string::to_string(notificationElement["event"].get_string().value),
        .notificationId=bsoncxx::string::to_string(notificationElement["notificationId"].get_string().value),
        .queueArn=bsoncxx::string::to_string(notificationElement["queueArn"].get_string().value),
        .lambdaArn=bsoncxx::string::to_string(notificationElement["lambdaArn"].get_string().value)
      };
      notifications.push_back(notification);
    }
  }

  void Bucket::FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

    oid = mResult.value()["_id"].get_oid().value.to_string();
    region = bsoncxx::string::to_string(mResult.value()["region"].get_string().value);
    name = bsoncxx::string::to_string(mResult.value()["name"].get_string().value);
    owner = bsoncxx::string::to_string(mResult.value()["owner"].get_string().value);
    versionStatus = BucketVersionStatusFromString(bsoncxx::string::to_string(mResult.value()["versionStatus"].get_string().value));
    created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
    modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

    bsoncxx::array::view notificationView{mResult.value()["notifications"].get_array().value};
    for (bsoncxx::array::element notificationElement : notificationView) {
      BucketNotification notification{
        .event=bsoncxx::string::to_string(notificationElement["event"].get_string().value),
        .notificationId=bsoncxx::string::to_string(notificationElement["notificationId"].get_string().value),
        .queueArn=bsoncxx::string::to_string(notificationElement["queueArn"].get_string().value),
        .lambdaArn=bsoncxx::string::to_string(notificationElement["lambdaArn"].get_string().value)
      };
      notifications.push_back(notification);
    }
  }

  std::string Bucket::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  /**
   * Stream provider.
   *
   * @return output stream
   */
  std::ostream &operator<<(std::ostream &os, const Bucket &q) {
    os << "Bucket={oid='" << q.oid << "', region='" << q.region << "', name='" << q.name << "', owner='" << q.owner << "' versionStatus='"
       << BucketVersionStatusToString(q.versionStatus) << "', created='" << Poco::DateTimeFormatter().format(q.created, Poco::DateTimeFormat::HTTP_FORMAT)
       << "', modified='" << Poco::DateTimeFormatter().format(q.created, Poco::DateTimeFormat::HTTP_FORMAT) << "'}";
    return os;
  }
}