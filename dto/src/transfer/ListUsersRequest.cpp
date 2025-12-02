//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/ListUsersRequest.h>

namespace AwsMock::Dto::Transfer {

    std::string ListUsersRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            Core::Bson::BsonUtils::SetIntValue(document, "MaxResults", maxResults);
            Core::Bson::BsonUtils::SetStringValue(document, "NextToken", nextToken);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ListUsersRequest::FromJson(const std::string &jsonString) {

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            serverId = Core::Bson::BsonUtils::GetStringValue(rootDocument, "ServerId");
            maxResults = Core::Bson::BsonUtils::GetIntValue(rootDocument, "MaxResults");
            nextToken = Core::Bson::BsonUtils::GetStringValue(rootDocument, "NextToken");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListUsersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListUsersRequest &r) {
        os << "ListUsersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
