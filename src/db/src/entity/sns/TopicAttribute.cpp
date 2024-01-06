//
// Created by vogje01 on 12/10/23.
//

#include <awsmock/entity/sns/TopicAttribute.h>

namespace AwsMock::Database::Entity::SNS {

  view_or_value<view, value> TopicAttribute::ToDocument() const {

    view_or_value<view, value> topicAttributeDoc = make_document(
      kvp("deliveryPolicy", deliveryPolicy),
      kvp("displayName", displayName),
      kvp("fifoTopic", fifoTopic),
      kvp("policy", policy),
      kvp("signatureVersion", signatureVersion),
      kvp("tracingConfig", tracingConfig),
      kvp("kmsMasterKeyId", kmsMasterKeyId),
      kvp("archivePolicy", archivePolicy),
      kvp("beginningArchiveTime", bsoncxx::types::b_date(std::chrono::milliseconds(beginningArchiveTime.timestamp().epochMicroseconds() / 1000))),
      kvp("contentBasedDeduplication", contentBasedDeduplication));

    return topicAttributeDoc;
  }

  void TopicAttribute::FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

    if(mResult->empty()) {
      return;
    }

    deliveryPolicy = mResult.value()["deliveryPolicy"].get_string().value;
    displayName = mResult.value()["displayName"].get_string().value;
    fifoTopic = mResult.value()["fifoTopic"].get_bool().value;
    policy = mResult.value()["policy"].get_string().value;
    signatureVersion = mResult.value()["signatureVersion"].get_string().value;
    tracingConfig = mResult.value()["tracingConfig"].get_string().value;
    kmsMasterKeyId = mResult.value()["kmsMasterKeyId"].get_string().value;
    archivePolicy = mResult.value()["archivePolicy"].get_string().value;
    beginningArchiveTime = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["beginningArchiveTime"].get_date().value) / 1000));
    contentBasedDeduplication = mResult.value()["contentBasedDeduplication"].get_bool().value;
  }

  Poco::JSON::Object TopicAttribute::ToJsonObject() const {
    Poco::JSON::Object jsonObject;
    jsonObject.set("deliveryPolicy", deliveryPolicy);
    jsonObject.set("displayName", displayName);
    jsonObject.set("fifoTopic", fifoTopic);
    jsonObject.set("policy", policy);
    jsonObject.set("signatureVersion", signatureVersion);
    jsonObject.set("tracingConfig", tracingConfig);
    jsonObject.set("kmsMasterKeyId", kmsMasterKeyId);
    jsonObject.set("contentBasedDeduplication", contentBasedDeduplication);
    return jsonObject;
  }

  void TopicAttribute::FromJsonObject(const Poco::JSON::Object::Ptr& jsonObject) {

    Core::JsonUtils::GetJsonValueString("deliveryPolicy", jsonObject, deliveryPolicy);
    Core::JsonUtils::GetJsonValueString("displayName", jsonObject, displayName);
    Core::JsonUtils::GetJsonValueBool("fifoTopic", jsonObject, fifoTopic);
    Core::JsonUtils::GetJsonValueString("policy", jsonObject, policy);
    Core::JsonUtils::GetJsonValueString("signatureVersion", jsonObject, signatureVersion);
    Core::JsonUtils::GetJsonValueString("tracingConfig", jsonObject, tracingConfig);
    Core::JsonUtils::GetJsonValueString("kmsMasterKeyId", jsonObject, kmsMasterKeyId);
    Core::JsonUtils::GetJsonValueBool("contentBasedDeduplication", jsonObject, contentBasedDeduplication);
  }

  std::string TopicAttribute::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const TopicAttribute &r) {
    os << "TopicAttribute={deliveryPolicy='" << r.deliveryPolicy << "' displayName='" << r.displayName << "' fifoTopic='" << r.fifoTopic << "' policy='" << r.policy << "' signatureVersion='" << r.signatureVersion
       << "' tracingConfig='" << r.tracingConfig << "' kmsMasterKeyId=" << r.kmsMasterKeyId << " archivePolicy='" << r.archivePolicy  << "' contentBasedDeduplication='" << r.contentBasedDeduplication
      << "' beginningArchiveTime='" << Poco::DateTimeFormatter::format(r.beginningArchiveTime, Poco::DateTimeFormat::ISO8601_FORMAT) << "'}";
    return os;
  }

} // namespace AwsMock::Database::Entity::SQS