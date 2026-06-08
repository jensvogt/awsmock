//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/ssm/Parameter.h>
#include <awsmock/repository/ssm/ISSMRepository.h>
#include <awsmock/utils/ConnectionPool.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief ssm MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMMongoRepository final : public ISSMRepository {

    public:
        /**
         * @brief Constructor
         */
        explicit SSMMongoRepository() = default;

        /**
         * @brief Check the existence of a parameter by name
         *
         * @param name parameter name
         * @return true if the key already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool parameterExists(const std::string &name) const override;

        /**
         * @brief Returns an SSM parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter getParameterById(const std::string &oid) const override;

        /**
         * @brief Returns a parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter getParameterById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns a parameter by name
         *
         * @param name parameter name
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter getParameterByName(const std::string &name) const override;

        /**
         * @brief List all parameters
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort column list
         * @return ParameterList
         */
        [[nodiscard]]
        Entity::SSM::ParameterList listParameters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns the total number of parameters
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return total number of parameters
         * @throws DatabaseException
         */
        [[nodiscard]]
        long countParameters(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Create a new parameter in the ssm parameter table
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter createParameter(Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Updates a parameter
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter updateParameter(Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Inserts or updates a parameter
         *
         * @param parameter parameter entity
         * @return inserted or updated parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter importParameter(Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Deletes a parameter
         *
         * @param parameter parameter entity
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteParameter(const Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Delete all parameters
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllParameters() const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SSM"};

        /**
         * @brief Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * Parameter collection name
         */
        static constexpr auto _parameterCollectionName = "ssm_parameter";
    };

} // namespace Awsmock::Database
