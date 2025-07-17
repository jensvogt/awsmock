//
// Created by vogje01 on 30/09/2023.
//

#include <awsmock/dto/docker/CreateContainerRequest.h>

namespace AwsMock::Dto::Docker {

    std::string CreateContainerRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Hostname", hostName);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Domainname", domainName);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "User", user);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Image", image);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "Tty", tty);

            if (!environment.empty()) {
                array envArray;
                for (const auto &env: environment) {
                    envArray.append(env);
                }
                rootDocument.append(kvp("Env", envArray));
            }

            // Exposed ports
            document exposedPortsObject;
            exposedPortsObject.append(kvp(containerPort, make_document()));
            rootDocument.append(kvp("ExposedPorts", exposedPortsObject));

            // Host config
            document hostConfigObject;

            // Port array
            document hostPortObject;
            hostPortObject.append(kvp("HostPort", hostPort));

            // Host array
            array hostArray;
            hostArray.append(hostPortObject);

            // Port bindings
            document portBindingsObject;
            portBindingsObject.append(kvp(containerPort, hostArray));
            hostConfigObject.append(kvp("PortBindings", portBindingsObject));

            // Hosts docker internal, localstack (for localstack compatibility) and awsmock are routed to the docker host
            array extraHostsArray;
            extraHostsArray.append("host.docker.internal:172.17.0.1");
            extraHostsArray.append("awsmock:172.17.0.1");
            extraHostsArray.append("localstack:172.17.0.1");

            hostConfigObject.append(kvp("ExtraHosts", extraHostsArray));
            hostConfigObject.append(kvp("NetworkMode", networkMode));
            rootDocument.append(kvp("HostConfig", hostConfigObject));

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateContainerRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateContainerRequest &r) {
        os << "CreateContainerRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Docker
