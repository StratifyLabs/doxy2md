
set(DEPENDENCIES
  JsonAPI
  TestAPI
  SysAPI
  FsAPI
  VarAPI
  ThreadAPI)

set(LOCAL_NAME ${PROJECT_NAME})

cmsdk_add_subdirectory(PRIVATE_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/src)
cmsdk_app_target(RELEASE ${LOCAL_NAME} "" release ${CMSDK_ARCH})
add_executable(${RELEASE_TARGET})
target_sources(${RELEASE_TARGET}
  PRIVATE
  ${PRIVATE_SOURCELIST}
  )

target_compile_definitions(${RELEASE_TARGET}
  PRIVATE
  -DVERSION=\"${CMAKE_PROJECT_VERSION}\"
  )

target_include_directories(${RELEASE_TARGET}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

target_compile_options(${RELEASE_TARGET}
  PRIVATE
  -O3
  )

set_property(TARGET ${RELEASE_TARGET} PROPERTY CXX_STANDARD 17)

cmsdk_app_add_arch_targets("${RELEASE_OPTIONS}" "${DEPENDENCIES}" 0)

install(
  PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/build_release_link/${LOCAL_NAME}_release_link.elf${CMSDK_EXEC_SUFFIX}
  DESTINATION ${CMSDK_LOCAL_PATH}/bin RENAME ${LOCAL_NAME}${CMSDK_EXEC_SUFFIX})
