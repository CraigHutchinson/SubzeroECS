# @note >=3.20 required for `GENERATED` attribute to be project-wide i.e. Version.h
cmake_minimum_required(VERSION 3.20)

# Register repository module paths
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CPM)

# Version.cmake automates our sematic build versioning
include(Version)

project( SubzeroECS VERSION ${VERSION_SEMANTIC}
  LANGUAGES CXX
)
set (CMAKE_CXX_STANDARD 20)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "Generating & building from within source is not allowed. Please configure git into a 'build' folder to run CMake from there."
  )
endif()

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.11.0")

find_package(fmt REQUIRED)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PRIVATE
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Collection.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Collection.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/CollectionRegistry.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Entity.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/EntityId.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/FreeIndexList.hpp 
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/GlobalUniqueIndex.hpp 
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Has.hpp 
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Logical.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Query.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/SubzeroECS.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/System.hpp 
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/Utility/StructOfVector.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/View.hpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/World.hpp
  PRIVATE 
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/CollectionRegistry.cpp
    ${PROJECT_SOURCE_DIR}/source/SubzeroECS/GlobalUniqueIndex.cpp
    )

set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 20)

target_compile_options(${PROJECT_NAME} 
PUBLIC 
  # Enforce standards conformance on MSVC
  "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->"
)

# Link dependencies
# target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# ---- Create an installable target ----
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "fmt 9.1.0"
)


add_subdirectory(test)