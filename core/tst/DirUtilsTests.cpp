//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_DIR_UTILS_TEST_H
#define AWMOCK_CORE_DIR_UTILS_TEST_H

// Local includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>

#define FILE_SIZE (100 * 1024)

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(DeleteDirTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);

        // act
        BOOST_CHECK_NO_THROW({ DirUtils::DeleteDirectory(dirName); });

        // assert
        BOOST_CHECK_EQUAL(DirUtils::DirectoryExists(dirName), false);
    }

    BOOST_AUTO_TEST_CASE(IsDirectoryTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);

        // act
        const bool result = DirUtils::IsDirectory(dirName);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(DeleteDirRecursiveTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);
        for (int i = 0; i < 3; i++) {
            FileUtils::CreateTempFile(dirName, "json", 100);
        }

        // act
        BOOST_CHECK_NO_THROW({ DirUtils::DeleteDirectory(dirName); });

        // assert
        BOOST_CHECK_EQUAL(DirUtils::DirectoryExists(dirName), false);
    }

    BOOST_AUTO_TEST_CASE(DirectoryFileCountTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);
        for (int i = 0; i < 3; i++) {
            FileUtils::CreateTempFile(dirName, "json", 100);
        }

        // act
        long result = 0;
        BOOST_CHECK_NO_THROW({ result = DirUtils::DirectoryCountFiles(dirName); });

        // assert
        BOOST_CHECK_EQUAL(result, 3);
    }

    BOOST_AUTO_TEST_CASE(DirectoryFileCountRecursivelyTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);
        const std::string dirName2 = DirUtils::CreateTempDir(dirName);
        for (int i = 0; i < 3; i++) {
            FileUtils::CreateTempFile(dirName2, "json", 100);
        }

        // act
        long result = 0;
        BOOST_CHECK_NO_THROW({ result = DirUtils::DirectoryCountFiles(dirName, true); });

        // assert
        BOOST_CHECK_EQUAL(result, 3);
    }

    BOOST_AUTO_TEST_CASE(DirectoryEmptyTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);

        // act
        bool result = false;
        BOOST_CHECK_NO_THROW({ result = DirUtils::DirectoryEmpty(dirName); });

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }


    BOOST_AUTO_TEST_CASE(ListFilesTest) {
        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);
        const std::string fileName1 = FileUtils::CreateTempFile(dirName, "json", 100);
        const std::string fileName2 = FileUtils::CreateTempFile(dirName, "txt", 100);
        const std::string fileName3 = FileUtils::CreateTempFile(dirName, "xml", 100);

        // act
        std::vector<std::string> result;
        BOOST_CHECK_NO_THROW({ result = DirUtils::ListFiles(dirName); });

        // assert
        BOOST_CHECK_EQUAL(result.empty(), false);
        BOOST_CHECK_EQUAL(std::ranges::find(result, fileName1) != result.end(), true);
        BOOST_CHECK_EQUAL(std::ranges::find(result, fileName2) != result.end(), true);
        BOOST_CHECK_EQUAL(std::ranges::find(result, fileName3) != result.end(), true);
    }

    BOOST_AUTO_TEST_CASE(ListFilesPatternTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string dirName = DirUtils::CreateTempDir(tempDir);
        const std::string fileName1 = FileUtils::CreateTempFile(dirName, "json", 100);
        const std::string fileName2 = FileUtils::CreateTempFile(dirName, "txt", 100);
        const std::string fileName3 = FileUtils::CreateTempFile(dirName, "xml", 100);

        // act
        std::vector<std::string> result;
        BOOST_CHECK_NO_THROW({ result = DirUtils::ListFilesByPattern(dirName, "^\\/tmp.*\\.xml$"); });

        // assert
        BOOST_CHECK_EQUAL(result.empty(), false);
        BOOST_CHECK_EQUAL(result.size(), 1);
        BOOST_CHECK_EQUAL(std::ranges::find(result, fileName3) != result.end(), true);
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_DIR_UTILS_TEST_H