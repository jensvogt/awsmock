//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_PORT_H
#define AWSMOCK_DTO_DOCKER_PORT_H

// C++ includes
#include <string>
#include <sstream>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/ServiceException.h>

namespace AwsMock::Dto::Docker {

    struct Port {

      /**
       * Constructor
       */
      Port(const Poco::JSON::Object::Ptr &object);

      /**
       * Private port, means port inside the container
       */
      int privatePort;

      /**
       * Public port, means port visible from docker host
       */
      int publicPort;

      /**
       * Port type (tcp,udp, etc.)
       */
      std::string type;

      /**
       * Convert to a JSON string
       *
       * @return JSON string
       */
      void FromJson(Poco::JSON::Object::Ptr object);

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const;

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const Port &c);

    };

} // namespace AwsMock::Dto::Docker

#endif // AWSMOCK_DTO_DOCKER_PORT_H