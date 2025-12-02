//
// Created by vogje01 on 10/23/23.
//

#ifndef AWSMOCK_AWSLOCAL_H
#define AWSMOCK_AWSLOCAL_H

// C++ includes
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Boost includes
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// AwsMock includes
#include <awsmock/core/SystemUtils.h>

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 4566
#define DEFAULT_PROFILE "default"
#define DEFAULT_BASE_URL "http://localhost:4566";

#ifdef _WIN32
#define AWS_CMD "C:/Program Files/Amazon/AWSCLIV2/aws.exe"
#else
#define AWS_CMD "/usr/local/bin/aws"
#endif
namespace AwsMock::AwsLocal {

    /**
     * @brief AWS local command
     *
     * @par
     * AwsLocal sends a AWS command to the AwsMock hosted on the local machine.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AwsLocal {

      public:

        /**
         * @brief Constructor
         */
        explicit AwsLocal() = default;

        /**
         * @brief Initialization
         *
         * @param vm vector of command line options
         */
        void Initialize(const boost::program_options::variables_map &vm);

        /**
         * @brief Main method
         *
         * @param command AWS command line command
         */
        void Run(std::vector<std::string> &command) const;

      private:

        /**
         * @brief Read the AWS config file
         *
         * @par
         * In case it finds an endpoint URL for the given profile, this URL is taken as base URL.
         */
        void ReadAwsConfigFile();

        /**
         * Host
         */
        std::string _host;

        /**
         * Port
         */
        int _port = DEFAULT_PORT;

        /**
         * Base URL
         */
        std::string _baseUrl;

        /**
         * User
         */
        std::string _user;

        /**
         * AWS profile
         */
        std::string _profile;
    };

}// namespace AwsMock::AwsLocal

#endif// AWSMOCK_AWSLOCAL_H
