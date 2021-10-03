cmake_minimum_required (VERSION 3.15.0)

project(ComInterfaceGeneratorProj LANGUAGES CXX)

add_executable(ComInterfaceGenerator)

set_property(TARGET ComInterfaceGenerator PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET ComInterfaceGenerator PROPERTY CXX_STANDARD 17)
add_subdirectory(Dependencies)
add_subdirectory(Src)

target_link_libraries(ComInterfaceGenerator PRIVATE CommandLine)
target_link_libraries(ComInterfaceGenerator PRIVATE libComInterfaceGenerator)