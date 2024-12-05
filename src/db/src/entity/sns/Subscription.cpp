//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sns/Subscription.h>

namespace AwsMock::Database::Entity::SNS {

    view_or_value<view, value> Subscription::ToDocument() const {

        view_or_value<view, value> subscriptionDoc = make_document(
                kvp("protocol", protocol),
                kvp("endpoint", endpoint),
                kvp("subscriptionArn", subscriptionArn));

        return subscriptionDoc;
    }

    void Subscription::FromDocument(std::optional<bsoncxx::document::view_or_value> mResult) {

        protocol = bsoncxx::string::to_string(mResult.value().view()["protocol"].get_string().value);
        endpoint = bsoncxx::string::to_string(mResult.value().view()["endpoint"].get_string().value);
        subscriptionArn = bsoncxx::string::to_string(mResult.value().view()["subscriptionArn"].get_string().value);
    }

    Poco::JSON::Object Subscription::ToJsonObject() const {

        try {

            Poco::JSON::Object jsonObject;
            jsonObject.set("protocol", protocol);
            jsonObject.set("endpoint", endpoint);
            jsonObject.set("subscriptionArn", subscriptionArn);
            return jsonObject;

        } catch (Poco::Exception &e) {
            log_error << e.message();
            throw Core::JsonException(e.message());
        }
    }

    void Subscription::FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject) {

        try {

            Core::JsonUtils::GetJsonValueString("protocol", jsonObject, protocol);
            Core::JsonUtils::GetJsonValueString("endpoint", jsonObject, endpoint);
            Core::JsonUtils::GetJsonValueString("subscriptionArn", jsonObject, subscriptionArn);

        } catch (Poco::Exception &e) {
            log_error << e.message();
            throw Core::JsonException(e.message());
        }
    }

    std::string Subscription::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Subscription &m) {
        os << "Subscription=" << bsoncxx::to_json(m.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SNS
