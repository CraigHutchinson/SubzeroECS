# @note >=3.18 required for `FetchContent`
# @note >=3.20 required for `GENERATED` attribute to be project-wide i.e. Version.h
cmake_minimum_required(VERSION 3.20) #FetchContent

include(CPM)
CPMAddPackage("gh:BareCpper/Version.cmake#master")

if ( NOT VERSION_SET )
    message( FATAL_ERROR "Version.cmake is required")
endif()