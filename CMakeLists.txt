cmake_minimum_required (VERSION 3.12)

set(SDK_DIRECTORY ${CMAKE_SOURCE_DIR}/SDK)
set(CMSDK_LOCAL_PATH ${SDK_DIRECTORY}/local)

set(CMAKE_MODULE_PATH ${CMSDK_LOCAL_PATH}/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/cmsdk-toolchain.cmake)
include(cmsdk)

set(DOXY2MD_PROJECT_VERSION 0.1 CACHE INTERNAL "doxy2md application version")
project(doxy2md
	LANGUAGES CXX C
	VERSION ${DOXY2MD_PROJECT_VERSION}
	)

include(CTest)

set(CMSDK_PULL_TARGET pull)

add_custom_target(pull
	COMMAND git pull
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(SDK SDK EXCLUDE_FROM_ALL)
add_subdirectory(app)
add_subdirectory(tests)

