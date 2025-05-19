//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_MODULE_MAPPER_H
#define AWSMOCK_DTO_MODULE_MAPPER_H

// AwsMock includes
#include <awsmock/dto/module/model/Module.h>
#include <awsmock/entity/module/Module.h>

namespace AwsMock::Dto::Module {

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

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_MODULE_MAPPER_H
