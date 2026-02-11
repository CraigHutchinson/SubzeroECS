# this file contains a list of tools that can be activated and downloaded on-demand each tool is
# enabled during configuration by passing an additional `-DUSE_<TOOL>=<VALUE>` argument to CMake

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# enables sanitizers support using the the `USE_SANITIZER` flag available values are: Address,
# Memory, MemoryWithOrigins, Undefined, Thread, Leak, 'Address;Undefined'
if(USE_SANITIZER OR USE_STATIC_ANALYZER)
  CPMAddPackage("gh:StableCoder/cmake-scripts@23.04")

  if(USE_SANITIZER)
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
  endif()

  if(USE_STATIC_ANALYZER)
    if("clang-tidy" IN_LIST USE_STATIC_ANALYZER)
      set(CLANG_TIDY
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CLANG_TIDY
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("iwyu" IN_LIST USE_STATIC_ANALYZER)
      set(IWYU
          ON
          CACHE INTERNAL ""
      )
    else()
      set(IWYU
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("cppcheck" IN_LIST USE_STATIC_ANALYZER)
      set(CPPCHECK
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CPPCHECK
          OFF
          CACHE INTERNAL ""
      )
    endif()

    include(${cmake-scripts_SOURCE_DIR}/tools.cmake)

    clang_tidy(${CLANG_TIDY_ARGS})
    include_what_you_use(${IWYU_ARGS})
    cppcheck(${CPPCHECK_ARGS})
  endif()
endif()

option(USE_CCACHE "Use CCache for faster incremental builds (requires ccache to be installed)" ON)

# enables CCACHE support through the USE_CCACHE flag possible values are: YES, NO or equivalent
if(USE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    message(STATUS "Build optimization: ccache found at ${CCACHE_PROGRAM}")
    # Set ccache as the compiler launcher for both C and C++
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE STRING "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE STRING "C++ compiler launcher")
    message(STATUS "Build optimization: Using ccache for faster incremental builds")
  else()
    message(WARNING "ccache was requested (USE_CCACHE=YES) but not found. Install ccache to enable compiler caching.")
    message(STATUS "  Windows: choco install ccache")
    message(STATUS "  Linux:   sudo apt install ccache  (or dnf/yum)")
    message(STATUS "  macOS:   brew install ccache")
  endif()
endif()

# Enable faster linkers (LLD for Clang, mold for GCC/Clang)
option(USE_LLD "Use LLVM's lld linker (faster linking)" OFF)
option(USE_MOLD "Use mold linker (fastest, Linux only)" OFF)

if(USE_MOLD AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
  # mold is the fastest linker, primarily for Linux
  add_link_options("-fuse-ld=mold")
  message(STATUS "Build optimization: Using mold linker")
elseif(USE_LLD AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # lld is LLVM's linker, works on multiple platforms
  add_link_options("-fuse-ld=lld")
  message(STATUS "Build optimization: Using lld linker")
endif()
