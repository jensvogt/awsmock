//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_DATABASETEST_H
#define AWMOCK_CORE_DATABASETEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/Database.h>

#define CONFIG_FILE "/tmp/awsmock.properties"

namespace AwsMock::Database {

    class DatabaseTest : public ::testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
        }

        Core::Configuration _configuration = Core::Configuration(TMP_CONFIGURATION_FILE);
        DatabaseBase _database = DatabaseBase();
    };

    /*TEST_F(DatabaseTest, ConstructorTest) {

      // arrange
      int count = 0;
      Poco::Data::Session session = _servicedatabase.GetSession();

      // act
      Poco::Data::Statement stmt(session);
      stmt << "SELECT COUNT(*) FROM module", into(count), now;

      // assert
      EXPECT_EQ(count, 4);
  }*/

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_DIRUTILSTEST_H