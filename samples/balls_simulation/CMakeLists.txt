cmake_minimum_required(VERSION 3.14...3.22)

project(BallsSimulation CXX)

# ---- Dependencies ----

CPMAddPackage(
  NAME SFML
  GIT_TAG 3.0.2
  GITHUB_REPOSITORY SFML/SFML
  OPTIONS
    "SFML_BUILD_AUDIO OFF"
    "SFML_BUILD_NETWORK OFF"
)

# ---- Executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(BallsSimulation ${sources})

set_target_properties(BallsSimulation PROPERTIES 
  CXX_STANDARD 20
  OUTPUT_NAME "BallsSimulation"
)

target_link_libraries(BallsSimulation 
  PRIVATE 
    SubzeroECS::SubzeroECS
    sfml-graphics
    sfml-window
    sfml-system
)

# Copy SFML DLLs to output directory on Windows
if(WIN32)
  add_custom_command(TARGET BallsSimulation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:sfml-graphics>
      $<TARGET_FILE:sfml-window>
      $<TARGET_FILE:sfml-system>
      $<TARGET_FILE_DIR:BallsSimulation>
  )
endif()
