if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/mockstream-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

# tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package mockstream)

install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT mockstream_Development
)

install(
    TARGETS mockstream_mockstream
    EXPORT mockstreamTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Allow package maintainers to freely override the path for the configs
set(
    mockstream_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE mockstream_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(mockstream_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${mockstream_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT mockstream_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${mockstream_INSTALL_CMAKEDIR}"
    COMPONENT mockstream_Development
)

install(
    EXPORT mockstreamTargets
    NAMESPACE mockstream::
    DESTINATION "${mockstream_INSTALL_CMAKEDIR}"
    COMPONENT mockstream_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()