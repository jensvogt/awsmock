//
// Created by vogje01 on 4/28/24.
//

#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database {

    void MongoUtils::SetDatetime(bsoncxx::builder::basic::document &document, const std::string &attribute, const system_clock::time_point datetime) {
        if (datetime.time_since_epoch().count() > 0) {
            document.append(kvp(attribute, bsoncxx::types::b_date(datetime)));
        } else {
            document.append(kvp(attribute, bsoncxx::types::b_null()));
        }
    }

    system_clock::time_point MongoUtils::GetDatetime(const std::optional<bsoncxx::document::view> &result, const std::string &attribute) {
        if (result->find(attribute) != result->end() && result.value()[attribute].type() != bsoncxx::type::k_null) {
            return bsoncxx::types::b_date(result.value()[attribute].get_date());
        }
        return system_clock::time_point::min();
    }

}// namespace AwsMock::Database