//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_FILE_UTILS_TEST_H
#define AWMOCK_CORE_FILE_UTILS_TEST_H

// Local includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>

#define FILE_SIZE (100 * 1024)

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(BasenameTest) {

        // arrange
        const std::string fileName = "/tmp/example.gif";

        // act
        const std::string result = FileUtils::GetBasename(fileName);

        // assert
        BOOST_CHECK_EQUAL(result.length(), 7);
        BOOST_CHECK_EQUAL(result, "example");
    }

    BOOST_AUTO_TEST_CASE(ExtensionTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);

        // act
        const std::string result = FileUtils::GetExtension(fileName);

        // assert
        BOOST_CHECK_EQUAL(result.length(), 3);
        BOOST_CHECK_EQUAL(result, "gif");
    }

    BOOST_AUTO_TEST_CASE(FileSizeTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);

        // act
        const long result = FileUtils::FileSize(fileName);

        // assert
        BOOST_CHECK_EQUAL(result, 100);
    }

    BOOST_AUTO_TEST_CASE(FileMoveToTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);
        const std::string targetFilename = tempDir + FileUtils::separator() + "test1/test2/test3/test4.gif";

        // act
        FileUtils::MoveTo(fileName, targetFilename);
        const bool result = FileUtils::FileExists(targetFilename);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(DeleteFileTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "gif", 100);

        // act
        BOOST_CHECK_NO_THROW({ FileUtils::RemoveFile(fileName); });

        // assert
        BOOST_CHECK_EQUAL(FileUtils::FileExists(fileName), false);
    }


    BOOST_AUTO_TEST_CASE(StripBasenameTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = tempDir + FileUtils::separator() + "testFile.txt";

        // act
        std::string basename;
        BOOST_CHECK_NO_THROW({ basename = FileUtils::StripBasePath(fileName); });

        // assert
        BOOST_CHECK_EQUAL(basename.empty(), false);
        BOOST_CHECK_EQUAL(basename, "testFile.txt");
    }

    BOOST_AUTO_TEST_CASE(GetParentPathTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = tempDir + FileUtils::separator() + "testFile.txt";

        // act
        const std::string tempPath = FileUtils::GetParentPath(fileName);

        // assert
        BOOST_CHECK_EQUAL(tempPath.empty(), false);
        BOOST_CHECK_EQUAL(tempPath, tempDir);
    }


    BOOST_AUTO_TEST_CASE(CreateTempFileTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();

        // act
        const std::string tempFile = FileUtils::CreateTempFile(tempDir, "json", FILE_SIZE);

        // assert
        BOOST_CHECK_EQUAL(tempFile.empty(), false);
        BOOST_CHECK_EQUAL(FileUtils::FileExists(tempFile), true);
        BOOST_CHECK_EQUAL(FILE_SIZE, FileUtils::FileSize(tempFile));
    }

    BOOST_AUTO_TEST_CASE(ReadFileTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName = FileUtils::CreateTempFile(tempDir, "txt", 100);

        // act
        std::string result;
        BOOST_CHECK_NO_THROW({ result = FileUtils::ReadFile(fileName); });

        // assert
        BOOST_CHECK_EQUAL(FileUtils::FileExists(fileName), true);
        BOOST_CHECK_EQUAL(100, result.size());
    }

    BOOST_AUTO_TEST_CASE(StreamCopierTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string fileName1 = FileUtils::CreateTempFile(tempDir, "txt", 100);
        const std::string fileName2 = tempDir + FileUtils::separator() + "testFile.txt";

        // act
        long result;
        BOOST_CHECK_NO_THROW({ result = FileUtils::StreamCopier(fileName1, fileName2); });

        // assert
        BOOST_CHECK_EQUAL(FileUtils::FileExists(fileName2), true);
        BOOST_CHECK_EQUAL(100, result);
    }

    BOOST_AUTO_TEST_CASE(GetContentTypePlainTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("txt", "This is a text file");

        // act
        const std::string contentType = FileUtils::GetContentType(tempFile, tempFile);

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "text/plain");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypeJsonTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("json", R"({"key":"value"})");

        // act
        const std::string contentType = FileUtils::GetContentType(tempFile, tempFile);

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "application/json");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypeXmlTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("xml", "<ONIXMessage xmlns=¸\"http://www.editeur.org/onix/3.0/reference\" release=\"3.0\"><Header><Sender><SenderName>APA</SenderName><ContactName>Violetta Tatar</ContactName></Sender></Header>");

        // act
        const std::string contentType = FileUtils::GetContentType(tempFile, tempFile);

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "application/xml");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypeJsonStringTest) {

        // arrange
        const std::string content = R"({"key":"value"})";

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicString(content);

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "application/json");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypeXmlStringTest) {

        // arrange
        const std::string content = R"(<?xml version="1.0" encoding="utf-8"?>)";

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicString(content);

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "text/xml");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypeJpgTest) {

        // arrange

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicFile("/tmp/9783911244381.jpg");

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "image/jpeg");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypeTifTest) {

        // arrange

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicFile("/tmp/7337529778404.tif");

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "image/tiff");
    }

    BOOST_AUTO_TEST_CASE(GetContentTypePdfTest) {

        // arrange

        // act
        const std::string contentType = FileUtils::GetContentTypeMagicFile("/tmp/dummy.pdf");

        // assert
        BOOST_CHECK_EQUAL(contentType.empty(), false);
        BOOST_CHECK_EQUAL(contentType, "application/pdf");
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_FILE_UTILS_TEST_H