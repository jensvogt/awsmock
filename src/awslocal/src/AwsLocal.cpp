//
// Created by vogje01 on 10/23/23.
//

#include <awsmock/awslocal/AwsLocal.h>

namespace AwsMock::AwsLocal {

    void AwsLocal::Initialize(const boost::program_options::variables_map &vm) {
        if (vm.count("host")) {
            _host = vm.at("host").as<std::string>();
            _baseUrl = "http://" + _host + ":" + std::to_string(_port);
        }
        if (vm.count("port")) {
            _port = vm.at("port").as<int>();
            _baseUrl = "http://" + _host + ":" + std::to_string(_port);
        }
        if (vm.count("profile")) {
            _profile = vm.at("profile").as<std::string>();
            ReadAwsConfigFile();
        }
    }


    void AwsLocal::Run(std::vector<std::string> &command) const {
        std::string out, err;
        command.push_back("--endpoint");
        command.push_back(_baseUrl);
        Core::SystemUtils::RunShellCommand(AWS_CMD, command, {}, out, err);
        if (!out.empty()) {
            std::cout << out;
        }
        if (!err.empty()) {
            std::cerr << err;
        }
    }

    void AwsLocal::ReadAwsConfigFile() {

        const std::string fileName = Core::SystemUtils::GetHomeDir() + "/.aws/config";
        boost::property_tree::ptree pt;
        read_ini(fileName, pt);
        if (pt.get_optional<std::string>(_profile + ".endpoint_url").is_initialized()) {
            _baseUrl = pt.get<std::string>(_profile + ".endpoint_url");
        }
    }
}// namespace AwsMock::AwsLocal