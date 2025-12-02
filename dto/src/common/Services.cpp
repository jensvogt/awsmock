//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/Services.h>

namespace AwsMock::Dto::Common {

    bool Services::HasService(const std::string &service) const {

        if (Core::StringUtils::EqualsIgnoreCase(service, "all")) {
            return true;
        }

        return std::ranges::find_if(serviceNames, [service](const std::string &t) {
                   return t == service;
               }) != serviceNames.end();
    }

    bool Services::Contains(const std::string &service) const {

        return std::ranges::find_if(serviceNames, [service](const std::string &t) {
                   return Core::StringUtils::EqualsIgnoreCase(t, service);
               }) != serviceNames.end();
    }

    std::string Services::ToJson() const {

        try {

            document document;

            if (!serviceNames.empty()) {
                array jsonArray;
                for (const auto &service: serviceNames) {
                    jsonArray.append(service);
                }
                document.append(kvp("services", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void Services::FromJson(const std::string &jsonString) {

        if (jsonString.empty()) {
            serviceNames.emplace_back("all");
            return;
        }

        try {

            if (const value document = bsoncxx::from_json(jsonString); document.view().find("services") != document.view().end()) {
                for (const view jsonArray = document["services"].get_array().value; const auto &service: jsonArray) {
                    serviceNames.emplace_back(service.get_string().value);
                }
                serviceNames.emplace_back("all");
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Services::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Services &r) {
        os << "Services=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Common
