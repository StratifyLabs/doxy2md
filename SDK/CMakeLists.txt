cmake_minimum_required(VERSION 3.12)

project(${PROJECT_NAME}-SDK)

set(API_IS_SDK TRUE CACHE INTERNAL "Build SDK projects")

set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
file(MAKE_DIRECTORY ${BASE_DIRECTORY})

message(STATUS "Base directory ${BASE_DIRECTORY}")

macro(sdk_add_subdirectory NAME BRANCH)
  cmsdk_git_clone_or_pull_branch(${BASE_DIRECTORY} ${NAME} https://github.com/StratifyLabs/${NAME}.git ${BRANCH})
  add_subdirectory(${BASE_DIRECTORY}/${NAME} ${NAME} EXCLUDE_FROM_ALL)
endmacro()

include(CTest)

sdk_add_subdirectory(API v1.5)
include(${BASE_DIRECTORY}/API/libraries/API.cmake)

sdk_add_subdirectory(JsonAPI v1.4)






