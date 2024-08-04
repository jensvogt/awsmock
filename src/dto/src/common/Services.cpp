//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/Services.h>

namespace AwsMock::Dto::Common {

    bool Services::HasService(const std::string &service) const {

        if (Core::StringUtils::EqualsIgnoreCase(service, "all")) {
            return true;
        }

        return find_if(serviceNames.begin(), serviceNames.end(), [service](const std::string &t) {
                   return t == service;
               }) != serviceNames.end();
    }

    bool Services::Contains(const std::string &service) const {

        return find_if(serviceNames.begin(), serviceNames.end(), [service](const std::string &t) {
                   return Core::StringUtils::EqualsIgnoreCase(t, service);
               }) != serviceNames.end();
    }

    std::string Services::ToJson() {

        try {
            Poco::JSON::Object jsonServices;
            Poco::JSON::Array jsonServiceArray;
            for (const auto &service: serviceNames) {
                jsonServiceArray.add(service);
            }
            jsonServices.set("services", jsonServiceArray);

            std::ostringstream os;
            jsonServices.stringify(os);
            return os.str();

        } catch (Poco::Exception &exc) {
            throw Core::ServiceException(exc.message());
        }
    }

    void Services::FromJson(const std::string &jsonString) {

        if (jsonString.empty()) {
            serviceNames.emplace_back("all");
            return;
        }

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jsonString);
        auto rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {

            // Services
            Poco::JSON::Array::Ptr jsonArray = rootObject->getArray("services");
            for (const auto &service: *jsonArray) {
                serviceNames.emplace_back(service.extract<std::string>());
            }

        } catch (Poco::Exception &exc) {
            throw Core::ServiceException(exc.message());
        }
    }
}// namespace AwsMock::Dto::Common
