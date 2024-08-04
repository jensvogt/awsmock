//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/cognito/model/UserPool.h>

namespace AwsMock::Dto::Cognito {

    std::string UserPool::ToJson() const {

        try {

            return Core::JsonUtils::ToJsonString(ToJsonObject());

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    Poco::JSON::Object UserPool::ToJsonObject() const {

        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Id", id);
            rootJson.set("Region", region);
            rootJson.set("Name", name);
            rootJson.set("UserPoolId", userPoolId);
            rootJson.set("Arn", arn);
            rootJson.set("CreationDate", Core::DateTimeUtils::ISO8601(created));
            rootJson.set("LastModifiedDate", Core::DateTimeUtils::ISO8601(modified));

            return rootJson;

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string UserPool::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UserPool &o) {
        os << "UserPool=" << o.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Cognito
