//
// Created by vogje01 on 20/04/2025.
//

#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Common {

    template<typename T>
    std::string BaseRequest<T>::ToString() {
        if constexpr (traits::has_ToJson_op_v<T>) {
            return t->ToJson();
        }
        return {};
    }
}