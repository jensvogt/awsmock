//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/module/mapper/Mapper.h>

namespace AwsMock::Dto::Module {

    Module::ModuleList Mapper::map(const std::vector<Database::Entity::Module::Module> &moduleList) {

        Module::ModuleList dtoList;
        for (const auto &module: moduleList) {
            dtoList.emplace_back(Module::FromDocument(module.ToDocument()));
        }
        return dtoList;
    }

}// namespace AwsMock::Dto::Module