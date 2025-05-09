//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_ZIP_UTILS_TEST_H
#define AWMOCK_CORE_ZIP_UTILS_TEST_H

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/TarUtils.h>

#define FILE_SIZE (100 * 1024)

namespace AwsMock::Core {

    class TarUtilsTest : public testing::Test {

      protected:

        void SetUp() override {
            tempDir = DirUtils::CreateTempDir();
            DirUtils::EnsureDirectory(tempDir);
        }

        void TearDown() override {
            DirUtils::DeleteDirectory(tempDir);
        }
        std::string tempDir;
    };

    TEST_F(TarUtilsTest, UncompressTest) {

        // arrange
        const std::string zipFileName = "/tmp/java-basic-1.0-SNAPSHOT.jar";

        // act
        EXPECT_NO_THROW({ TarUtils::Unzip(zipFileName, tempDir); });
        const long count = DirUtils::DirectoryCountFiles(tempDir);

        // assert
        EXPECT_EQ(71, count);
    }


}// namespace AwsMock::Core

#endif// AWMOCK_CORE_ZIP_UTILS_TEST_H