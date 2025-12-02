// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if !defined(BSONCXX_V1_CONFIG_VERSION_HPP)
#define BSONCXX_V1_CONFIG_VERSION_HPP

// clang-format off
#define BSONCXX_VERSION_STRING "4.2.0-pre"
#define BSONCXX_VERSION_MAJOR 4
#define BSONCXX_VERSION_MINOR 2
#define BSONCXX_VERSION_PATCH 0
#define BSONCXX_VERSION_EXTRA -pre
// clang-format on

///
/// @file
/// Provides macros describing the bsoncxx library version.
///

///
/// @def BSONCXX_VERSION_STRING
/// @hideinitializer
/// This library's full version number as a string literal (e.g. `"1.2.3-extra"`).
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///

///
/// @def BSONCXX_VERSION_MAJOR
/// @hideinitializer
/// This library's API major version number as an integer literal (e.g. `1`).
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///

///
/// @def BSONCXX_VERSION_MINOR
/// @hideinitializer
/// This library's API minor version number as an integer literal (e.g. `2`).
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///

///
/// @def BSONCXX_VERSION_PATCH
/// @hideinitializer
/// This library's API patch version number as an integer literal (e.g. `3`).
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///

///
/// @def BSONCXX_VERSION_EXTRA
/// @hideinitializer
/// This library's API version number suffix if any as raw text (e.g. `-extra`).
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///

#endif // BSONCXX_V1_CONFIG_VERSION_HPP
