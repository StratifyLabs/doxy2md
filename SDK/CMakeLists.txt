cmake_minimum_required(VERSION 3.12)

project(${PROJECT_NAME}-SDK)

set(API_IS_SDK TRUE CACHE INTERNAL "Build SDK projects")

set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
file(MAKE_DIRECTORY ${BASE_DIRECTORY})

message(STATUS "Base directory ${BASE_DIRECTORY}")

# refresh the bootstrap code if needed
execute_process(
  COMMAND git pull
  WORKING_DIRECTORY ${BASE_DIRECTORY}/CMakeSDK
)

execute_process(
  COMMAND cmake --build . --target install
  WORKING_DIRECTORY ${BASE_DIRECTORY}/CMakeSDK/cmake_link
)

macro(sdk_add_subdirectory NAME BRANCH)
  cmsdk_git_clone_or_pull_branch(${BASE_DIRECTORY} ${NAME} https://github.com/StratifyLabs/${NAME}.git ${BRANCH})
  add_subdirectory(${BASE_DIRECTORY}/${NAME} ${NAME} EXCLUDE_FROM_ALL)
endmacro()

include(CTest)

option(SDK_IS_TEST "Enable all SDK API tests" OFF)
message(STATUS "Enable all tests? " ${SDK_IS_TEST})
if(SDK_IS_TEST)
  message(STATUS "Enabling tests for Api, Inet, Json, and Crypto")
  set(API_IS_TEST ON CACHE INTERNAL "Enable API Test")
  set(INET_API_IS_TEST ON CACHE INTERNAL "Enable InetAPI Test")
  set(JSON_API_IS_TEST ON CACHE INTERNAL "Enable JsonAPI Test")
  set(CRYPTO_API_IS_TEST ON CACHE INTERNAL "Enable CryptoAPI Test")
endif()

sdk_add_subdirectory(API feature/cmsdk-port-v2.0)
include(${BASE_DIRECTORY}/API/libraries/API.cmake)

sdk_add_subdirectory(JsonAPI feature/cmsdk-port-v2.0)






