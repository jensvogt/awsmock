//
// Created by vogje01 on 23/04/2025.
//

#ifndef AWSMOCK_DTO_COMMON_MAPPER_H
#define AWSMOCK_DTO_COMMON_MAPPER_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/SortColumn.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Common {

    /**
     * @brief Maps an sort column DTO entity to the corresponding entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
       * @brief Maps a single sort coumn DTo to an sort column entity
       *
       * @param sortColumnDto sort column DTO
       * @return sort column entity.
       */
        static Database::SortColumn map(const SortColumn &sortColumnDto) {
            Database::SortColumn sortColumnEntity;
            sortColumnEntity.column = sortColumnDto.column;
            sortColumnEntity.sortDirection = sortColumnDto.sortDirection;
            return sortColumnEntity;
        }

        /**
         * @brief Maps a single sort coumn DTo to a sort column entity
         *
         * @param sortColumnDtos sort column DTOs
         * @return sort column entity.
         */
        static std::vector<Database::SortColumn> map(const std::vector<SortColumn> &sortColumnDtos) {
            std::vector<Database::SortColumn> sortColumnEntities;
            for (const auto &s: sortColumnDtos) {
                sortColumnEntities.emplace_back(map(s));
            }
            return sortColumnEntities;
        }
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_MAPPER_H
