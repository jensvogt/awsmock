#cmake_minimum_required(VERSION 3.22)
#project(s3-notification-lambda LANGUAGES CXX)

set(BINARY s3-notification-lambda)
set(SOURCES src/main.cpp)

find_package(aws-lambda-runtime REQUIRED)
find_package(AWSSDK COMPONENTS s3 REQUIRED)

add_executable(${BINARY} ${SOURCES})
target_include_directories(${BINARY} PUBLIC ./include ${EXTERNAL_INCLDUE_DIR})
target_link_directories(${BINARY} PUBLIC /usr/lib /usr/local/lib)
target_link_libraries(${BINARY} PUBLIC AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES})
aws_lambda_package_target(${BINARY})

install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/lambda)

