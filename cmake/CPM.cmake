# SPDX-License-Identifier: MIT
#
# SPDX-FileCopyrightText: Copyright (c) 2019-2023 Lars Melchior and contributors

set(CPM_DOWNLOAD_VERSION 0.42.0)
set(CPM_HASH_SUM "2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a")

# Implicit CPM configure to use a user-pfoile CPM cache
# @note This can be overriden by defining CPM_SOURCE_CACHE or the CPM_SOURCE_CACHE environment variable
if(NOT DEFINED CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})

  if(WIN32)
    set(_cpm_home "$ENV{USERPROFILE}/.cpm")
  else()
    set(_cpm_home "$ENV{HOME}/.cpm")
  endif()
  file(TO_CMAKE_PATH "${_cpm_home}" CPM_SOURCE_CACHE)
  set(CPM_SOURCE_CACHE "${CPM_SOURCE_CACHE}" CACHE PATH "Download cache used by CPM")
  message(WARNING "CPM_SOURCE_CACHE not defined; defaulting to ${CPM_SOURCE_CACHE}.
   Set CPM_SOURCE_CACHE or the CPM_SOURCE_CACHE environment variable to override this location.")
endif()


if(CPM_PATH) # CPM_PATH may be provided on command-line
  set(CPM_DOWNLOAD_LOCATION "${CPM_PATH}/CPM.cmake")
elseif(DEFINED ENV{CPM_PATH}) # CPM_PATH may be provided from script environment
  file(TO_CMAKE_PATH "$ENV{CPM_PATH}/CPM.cmake" CPM_DOWNLOAD_LOCATION)
elseif(CPM_SOURCE_CACHE) 
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD
       https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION} EXPECTED_HASH SHA256=${CPM_HASH_SUM}
  )
endif()

include(${CPM_DOWNLOAD_LOCATION})