cmake_minimum_required(VERSION 3.18)

include(CPM)

CPMAddPackage(
	NAME benchmark
	GITHUB_REPOSITORY google/benchmark
	VERSION 1.9.4
	EXCLUDE_FROM_ALL true
	OPTIONS
		"BENCHMARK_ENABLE_TESTING OFF"
		"BENCHMARK_ENABLE_GTEST_TESTS OFF"
)

if(benchmark_ADDED)
	# Aliases matching find_package(benchmark) naming
	if(NOT TARGET benchmark::benchmark)
		add_library(benchmark::benchmark ALIAS benchmark)
	endif()
	
	if(NOT TARGET benchmark::benchmark_main)
		add_library(benchmark::benchmark_main ALIAS benchmark_main)
	endif()
endif()
