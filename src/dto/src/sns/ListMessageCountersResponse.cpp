//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sns/ListMessageCountersResponse.h>

namespace AwsMock::Dto::SNS {

    std::string ListMessageCountersResponse::ToJson() const {

        try {
            document document;
            if (!messages.empty()) {
                array messageArrayJson;
                for (const auto &message: messages) {
                    messageArrayJson.append(message.ToDocument());
                }
                document.append(kvp("Messages", messageArrayJson));
            }
            document.append(kvp("Total", bsoncxx::types::b_int64(total)));

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListMessageCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListMessageCountersResponse &r) {
        os << "ListMessageCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SNS
