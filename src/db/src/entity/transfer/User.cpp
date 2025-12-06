//
// Created by vogje01 on 13/09/2023.
//

#include <awsmock/entity/transfer/Transfer.h>

namespace AwsMock::Database::Entity::Transfer {

    view_or_value<view, value> User::ToDocument() const {

        document userDoc;
        userDoc.append(kvp("arn", arn));
        userDoc.append(kvp("role", role));
        userDoc.append(kvp("userName", userName));
        userDoc.append(kvp("password", password));
        userDoc.append(kvp("homeDirectory", homeDirectory));

        return userDoc.extract();
    }

    void User::FromDocument(const view mResult) {
        //
        //        for (auto &v: mResult) {
        //            arn = bsoncxx::string::to_string(v["arn"].get_string().value);
        //            role = bsoncxx::string::to_string(v["role"].get_string().value);
        //            userName = bsoncxx::string::to_string(v["userName"].get_string().value);
        //            password = bsoncxx::string::to_string(v["password"].get_string().value);
        //            homeDirectory = bsoncxx::string::to_string(v["homeDirectory"].get_string().value);
        //        }
    }

    std::string User::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const User &u) {
        os << "User=" << to_json(u.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::Transfer