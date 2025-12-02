//
// Created by vogje01 on 30/05/2023.
//

// AwsMock includes
#include <awsmock/dto/transfer/GetServerDetailsResponse.h>

namespace AwsMock::Dto::Transfer {

    std::string GetServerDetailsResponse::ToJson() const {

        try {

            document document;
            document.append(kvp("server", server.ToDocument()));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetServerDetailsResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetServerDetailsResponse &r) {
        os << "GetServerDetailsResponse=" + r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
