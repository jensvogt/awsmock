//
// Created by vogje01 on 2/11/25.
//

#include <awsmock/dto/docker/model/HostConfig.h>

namespace AwsMock::Dto::Docker {

    void HostConfig::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            FromDocument(document.view());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void HostConfig::FromDocument(const view_or_value<view, value> &document) {

        try {

            networkMode = Core::Bson::BsonUtils::GetStringValue(document, "NetworkMode");

            // Names array
            if (document.view().find("PortBindings") != document.view().end()) {
                for (const view portBindingsView = document.view()["PortBindings"].get_document().value; const bsoncxx::document::element &portBindingsElement: portBindingsView) {
                    std::string name = bsoncxx::string::to_string(portBindingsElement.key());
                    std::vector<Port> ports;
                    for (const view value = portBindingsView[name].get_array().value; const bsoncxx::document::element &portBindingElement: value) {
                        Port port;
                        port.hostIp = Core::Bson::BsonUtils::GetStringValue(portBindingElement.get_document(), "HostIp");
                        port.publicPort = std::stoi(Core::Bson::BsonUtils::GetStringValue(portBindingElement.get_document(), "HostPort"));
                        ports.push_back(port);
                    }
                    portBindings.portMap[name] = ports;
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Docker