//
// Created by vogje01 on 5/12/24.
//

#include <awsmock/service/s3/S3HashCreator.h>

namespace AwsMock::Service {

    void S3HashCreator::operator()(std::vector<std::string> &algorithms, Database::Entity::S3::Object &object) {

        Core::Configuration &configuration = Core::Configuration::instance();
        std::string dataDir = configuration.getString("awsmock.data.dir", DEFAULT_DATA_DIR);

        std::string filename = dataDir + Poco::Path::separator() + object.internalName;
        for (const auto &algorithm: algorithms) {
            if (algorithm == "SHA1") {
                object.sha1sum = Core::Crypto::GetSha1FromFile(filename);
            } else if (algorithm == "SHA256") {
                object.sha256sum = Core::Crypto::GetSha256FromFile(filename);
            } else if (algorithm == "MD5") {
                object.sha256sum = Core::Crypto::GetMd5FromFile(filename);
            }
            Database::S3Database::instance().UpdateObject(object);
            log_debug << "Calculated hashes, key: " << object.key << " hash: " << algorithm;
        }
    }

}// namespace AwsMock::Service
