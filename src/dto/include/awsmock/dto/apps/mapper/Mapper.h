//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_APPS_MAPPER_H
#define AWSMOCK_DTO_APPS_MAPPER_H

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/entity/apps/Application.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps an application entity to an application DTO
         *
         * @param applicationEntity application entity
         * @return Application DTO
         * @see AwsMock::Dto::Apps::Application
         */
        static Application map(const Database::Entity::Apps::Application &applicationEntity);

        /**
         * @brief Maps a list of application entities to a list of application DTOs
         *
         * @param applicationEntities list of application entity
         * @return std::vector<ApplicationDTO>
         * @see AwsMock::Dto::Apps::Application
         */
        static std::vector<Application> map(const std::vector<Database::Entity::Apps::Application> &applicationEntities);

        /**
         * @brief Maps an application DTO to an application entity
         *
         * @param applicationDto application DTO
         * @return Application DTO
         * @see AwsMock::Dto::Apps::Application
         */
        static Database::Entity::Apps::Application map(const Application &applicationDto);

        /**
         * @brief Maps a list of application DTOs to a list of application entities
         *
         * @param applicationDtos list of application DTOs
         * @return std::vector<ApplicationEntity>
         * @see AwsMock::Dto::Apps::Application
         */
        static std::vector<Database::Entity::Apps::Application> map(const std::vector<Application> &applicationDtos);
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_MAPPER_H
