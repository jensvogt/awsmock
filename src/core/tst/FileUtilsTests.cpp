//
// Created by vogje01 on 02/06/2023.
//

#include <awsmock/core/SystemUtils.h>
#ifndef AWMOCK_CORE_FILE_UTILS_TEST_H
#define AWMOCK_CORE_FILE_UTILS_TEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>

#define FILE_SIZE (100 * 1024)

namespace AwsMock::Core {

    class FileUtilsTest : public testing::Test {

      protected:

        void SetUp() override {
            tempDir = DirUtils::CreateTempDir();
        }

        void TearDown() override {
            DirUtils::DeleteDirectory(tempDir);
        }
        std::string tempDir;
    };

    TEST_F(FileUtilsTest, BasenameTest) {

        // arrange
        const std::string fileName = tempDir + Core::FileUtils::separator() + "example.gif";

        // act
        const std::string result = FileUtils::GetBasename(fileName);

        // assert
        EXPECT_EQ(result.length(), 7);
        EXPECT_EQ(result, "example");
    }

    TEST_F(FileUtilsTest, ExtensionTest) {
        // arrange
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);

        // act
        const std::string result = FileUtils::GetExtension(fileName);

        // assert
        EXPECT_EQ(result.length(), 3);
        EXPECT_EQ(result, "gif");
    }

    TEST_F(FileUtilsTest, FileSizeTest) {
        // arrange
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);

        // act
        const long result = FileUtils::FileSize(fileName);

        // assert
        EXPECT_EQ(result, 100);
    }

    TEST_F(FileUtilsTest, FileMoveToTest) {

        // arrange
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);
        const std::string targetFilename = tempDir + FileUtils::separator() + "test1/test2/test3/test4.gif";

        // act
        FileUtils::MoveTo(fileName, targetFilename);
        const bool result = FileUtils::FileExists(targetFilename);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(FileUtilsTest, DeleteFileTest) {

        // arrange
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);

        // act
        EXPECT_NO_THROW({ FileUtils::DeleteFile(fileName); });

        // assert
        EXPECT_FALSE(FileUtils::FileExists(fileName));
    }

    TEST_F(FileUtilsTest, StripBasenameTest) {

        // arrange
        const std::string fileName = tempDir + FileUtils::separator() + "testFile.txt";

        // act
        std::string basename;
        EXPECT_NO_THROW({ basename = FileUtils::StripBasePath(fileName); });

        // assert
        EXPECT_FALSE(basename.empty());
        EXPECT_STREQ(basename.c_str(), "testFile.txt");
    }

    TEST_F(FileUtilsTest, GetParentPathTest) {

        // arrange
        const std::string fileName = tempDir + FileUtils::separator() + "testFile.txt";

        // act
        const std::string tempPath = FileUtils::GetParentPath(fileName);

        // assert
        EXPECT_FALSE(tempPath.empty());
        EXPECT_TRUE(tempPath == tempDir);
    }

    TEST_F(FileUtilsTest, CreateTempFileTest) {
        // arrange

        // act
        const std::string tempFile = FileUtils::CreateTempFile(tempDir, "json", FILE_SIZE);

        // assert
        EXPECT_FALSE(tempFile.empty());
        EXPECT_TRUE(FileUtils::FileExists(tempFile));
        EXPECT_EQ(FILE_SIZE, FileUtils::FileSize(tempFile));
    }

    TEST_F(FileUtilsTest, ReadFileTest) {

        // arrange
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "txt", 100);

        // act
        std::string result;
        EXPECT_NO_THROW({ result = FileUtils::ReadFile(fileName); });

        // assert
        EXPECT_TRUE(FileUtils::FileExists(fileName));
        EXPECT_EQ(100, result.size());
    }

    TEST_F(FileUtilsTest, StripChunkSignatureTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("txt", "b;chunk-signature=654753fda\ntest\nb;chunk-signature=654753fda\n");

        // act
        FileUtils::StripChunkSignature(tempFile);

        // assert
        EXPECT_FALSE(tempFile.empty());
        EXPECT_TRUE(FileUtils::FileExists(tempFile));
        EXPECT_EQ(4, FileUtils::FileSize(tempFile));
    }

    TEST_F(FileUtilsTest, GetContentTypePlainTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("txt", "This is a text file");

        // act
        const std::string contentType = FileUtils::GetContentType(tempFile, tempFile);

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(Core::StringUtils::Equals(contentType, "text/plain"));
    }

    TEST_F(FileUtilsTest, GetContentTypeJsonTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("json", R"({"key":"value"})");

        // act
        const std::string contentType = FileUtils::GetContentType(tempFile, tempFile);

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(contentType == "application/json");
    }

    TEST_F(FileUtilsTest, GetContentTypeXmlTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("xml", "<ONIXMessage xmlns=¸\"http://www.editeur.org/onix/3.0/reference\" release=\"3.0\"><Header><Sender><SenderName>APA</SenderName><ContactName>Violetta Tatar</ContactName></Sender></Header>");

        // act
        const std::string contentType = FileUtils::GetContentType(tempFile, tempFile);

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(contentType == "application/xml");
    }

    TEST_F(FileUtilsTest, GetContentTypeJsonStringTest) {

        // arrange
        const std::string content = R"({"key":"value"})";

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicString(content);

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(contentType == "application/json");
    }

    TEST_F(FileUtilsTest, GetContentTypeXmlStringTest) {

        // arrange
        const std::string content = R"(<?xml version="1.0" encoding="utf-8"?>)";

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicString(content);

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(contentType == "text/plain");
    }

    TEST_F(FileUtilsTest, GetContentTypeJpgTest) {

        // arrange

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicFile("./resources/images/9783911244381.jpg");

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(contentType == "image/jpeg");
    }

    TEST_F(FileUtilsTest, GetContentTypeTifTest) {

        // arrange

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicFile("./resources/images/7337529778404.tif");

        // assert
        EXPECT_FALSE(contentType.empty());
        EXPECT_TRUE(contentType == "image/tiff");
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_FILE_UTILS_TEST_H