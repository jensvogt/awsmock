//
// Created by vogje01 on 5/10/24.
//

#pragma once
// AwsMock includes
#include <awsmock/dto/module/model/Module.h>
#include <awsmock/entity/module/Module.h>

namespace Awsmock::Dto::Module {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a module entity list to a module DTO
         *
         * @param moduleList module entity list
         * @return module DTO list
         */
        static Module::ModuleList map(const std::vector<Database::Entity::Module::Module> &moduleList);
    };

}// namespace Awsmock::Dto::Module

