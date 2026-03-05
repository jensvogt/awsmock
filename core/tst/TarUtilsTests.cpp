//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_ZIP_UTILS_TEST_H
#define AWMOCK_CORE_ZIP_UTILS_TEST_H

// Local includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/ZipUtils.h>

#define FILE_SIZE (100 * 1024)

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(UncompressTest) {

        // arrange
        const std::string tempDir = DirUtils::CreateTempDir();
        const std::string zipFileName = "/tmp/java-basic-1.0-SNAPSHOT.jar";

        // act
        BOOST_CHECK_NO_THROW({ ZipUtils::Unzip(zipFileName, tempDir); });
        const long count = DirUtils::DirectoryCountFiles(tempDir);

        // assert
        BOOST_CHECK_EQUAL(71, count);
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_ZIP_UTILS_TEST_H