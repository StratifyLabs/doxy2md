set(SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDK)
set(DEPENDENCIES_DIRECTORY ${SDK_DIRECTORY}/dependencies)

file(MAKE_DIRECTORY ${DEPENDENCIES_DIRECTORY})
file(REMOVE_RECURSE ${DEPENDENCIES_DIRECTORY}/CMakeSDK)

message(STATUS "cloning CMakeSDK project")

execute_process(
  COMMAND git clone --branch v2.0 https://github.com/StratifyLabs/CMakeSDK.git
  WORKING_DIRECTORY ${DEPENDENCIES_DIRECTORY}
)

option(IS_ARM_CROSS_COMPILE "Setup the system to cross compile to Stratify OS" OFF)

# This is used on the CI server, but can be run locally as well
# This is currently NOT used with IS_ARM_CROSS_COMPILE=ON
option(IS_BUILD_AND_TEST "Build and run the API tests" OFF)

if(IS_ARM_CROSS_COMPILE)
  set(BOOTSTRAP_SCRIPT sos-bootstrap)
else()
  set(BOOTSTRAP_SCRIPT bootstrap)
endif()

message(STATUS "Running ${BOOTSTRAP_SCRIPT} using SDK path: ${SDK_DIRECTORY}")

execute_process(
  COMMAND cmake -DCMSDK_SDK_PATH=${SDK_DIRECTORY} -P ${DEPENDENCIES_DIRECTORY}/CMakeSDK/scripts/${BOOTSTRAP_SCRIPT}.cmake
)

if(IS_BUILD_AND_TEST)
  message(STATUS "Build and run API Tests")
  set(BUILD_DIR cmake_link)
  set(BUILD_DIR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR})

  file(MAKE_DIRECTORY ${BUILD_DIR_PATH})
  option(IS_GENERATOR_MAKE "Generator to use for building" OFF)

  if(${IS_GENERATOR_MAKE})
    set(GENERATOR "Unix Makefiles")
  else()
    set(GENERATOR Ninja)
  endif()

  execute_process(
    COMMAND cmake -DSDK_IS_TEST=ON .. -G${GENERATOR}
    WORKING_DIRECTORY ${BUILD_DIR_PATH}
    RESULT_VARIABLE CMSDK_RESULT
  )

  if(${CMSDK_RESULT})
    message(FATAL_ERROR "Failed to run CMake")
  endif()

  execute_process(
    COMMAND cmake --build . --target all -- -j8
    WORKING_DIRECTORY ${BUILD_DIR_PATH}
    RESULT_VARIABLE BUILD_SDK_RESULT
  )

  if(${BUILD_SDK_RESULT})
    message(FATAL_ERROR "Failed to build")
  endif()

  execute_process(
    COMMAND cmake --build . --target API_test -- -j8
    WORKING_DIRECTORY ${BUILD_DIR_PATH}
    RESULT_VARIABLE BUILD_API_TEST_RESULT
  )

  if(${BUILD_API_TEST_RESULT})
    message(FATAL_ERROR "Failed to build API test")
  endif()

  execute_process(
    COMMAND ctest -VV
    WORKING_DIRECTORY ${BUILD_DIR_PATH}
    RESULT_VARIABLE TEST_OUTPUT_RESULT
  )

  if(NOT ${TEST_OUTPUT_RESULT} EQUAL "0")
    message(FATAL_ERROR "Test failed to complete successfully")
  endif()

endif()


