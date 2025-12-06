//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/module/ExportInfrastructureResponse.h>

namespace AwsMock::Dto::Module {

    std::string ExportInfrastructureResponse::ToJson() const {

        try {

            document document;
            document.append(kvp("onlyObjects", includeObjects));
            document.append(kvp("prettyPrint", prettyPrint));
            document.append(kvp("infrastructure", infrastructure.ToDocument()));
            return bsoncxx::to_json(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ExportInfrastructureResponse::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, ExportInfrastructureResponse &r) {
        os << "ExportInfrastructureResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Module