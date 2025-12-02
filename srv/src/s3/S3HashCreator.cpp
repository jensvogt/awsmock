//
// Created by vogje01 on 5/12/24.
//

#include <awsmock/service/s3/S3HashCreator.h>

namespace AwsMock::Service {
    void S3HashCreator::operator()(std::vector<std::string> &algorithms, Database::Entity::S3::Object &object) const {
        const std::string dataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");

        const std::string filename = dataDir + "/" + object.internalName;
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
