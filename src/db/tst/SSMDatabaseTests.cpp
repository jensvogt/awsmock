//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_REPOSITORY_SSM_DATABASE_TESTS_H
#define AWSMOCK_REPOSITORY_SSM_DATABASE_TESTS_H

// C++ standard includes
#include <iostream>
#include <vector>

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SSMDatabase.h>

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

#define SERVICE "test-module"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    class SSMDatabaseTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        void TearDown() override {
            _ssmDatabase.DeleteAllParameters();
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        SSMDatabase &_ssmDatabase = SSMDatabase::instance();
    };

    TEST_F(SSMDatabaseTest, ParameterCreateTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};

        // act
        Entity::SSM::Parameter result = _ssmDatabase.CreateParameter(parameter);

        // assert
        EXPECT_FALSE(result.oid.empty());
        EXPECT_TRUE(result.parameterName == "parameter-name");
        EXPECT_EQ(1, _ssmDatabase.CountParameters());
    }

    TEST_F(SSMDatabaseTest, ParameterExistsTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        Entity::SSM::Parameter createResult = _ssmDatabase.CreateParameter(parameter);

        // act
        bool result1 = _ssmDatabase.ParameterExists(parameter.parameterName);
        bool result2 = _ssmDatabase.ParameterExists("blabla");

        // assert
        EXPECT_TRUE(result1);
        EXPECT_FALSE(result2);
    }

    TEST_F(SSMDatabaseTest, ParameterGetByNameTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        Entity::SSM::Parameter createResult = _ssmDatabase.CreateParameter(parameter);

        // act
        Entity::SSM::Parameter result = _ssmDatabase.GetParameterByName(parameter.parameterName);

        // assert
        EXPECT_TRUE(result.parameterName == "parameter-name");
    }

    TEST_F(SSMDatabaseTest, ParameterGetByIdTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        Entity::SSM::Parameter createResult = _ssmDatabase.CreateParameter(parameter);
        bsoncxx::oid oid(createResult.oid);

        // act
        Entity::SSM::Parameter result = _ssmDatabase.GetParameterById(oid);

        // assert
        EXPECT_TRUE(result.parameterName == "parameter-name");
    }

    TEST_F(SSMDatabaseTest, ParameterUpdateTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        parameter = _ssmDatabase.CreateParameter(parameter);
        parameter.description = "new description";

        // act
        Entity::SSM::Parameter result = _ssmDatabase.UpdateParameter(parameter);

        // assert
        EXPECT_TRUE(result.description == "new description");
    }

    /*TEST_F(SSMDatabaseTest, ModuleListTest) {

        // arrange
        Entity::Module::Module module = {.name = SERVICE, .state = Entity::Module::ModuleState::RUNNING};
        _moduleDatabase.CreateModule(module);

        // act
        Entity::Module::ModuleList result = _moduleDatabase.ListModules();

        // assert
        EXPECT_EQ(1, result.size());
    }*/

    TEST_F(SSMDatabaseTest, ParameterDeleteTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        parameter = _ssmDatabase.CreateParameter(parameter);

        // act
        _ssmDatabase.DeleteParameter(parameter);
        long count = _ssmDatabase.CountParameters();

        // assert
        EXPECT_EQ(0, count);
    }

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SSM_DATABASE_TESTS_H