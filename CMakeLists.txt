cmake_minimum_required (VERSION 3.15.0)

project(ComInterfaceGeneratorProj LANGUAGES CXX)

add_executable(ComInterfaceGenerator)


set_property(TARGET ComInterfaceGenerator PROPERTY CXX_STANDARD 17)
add_subdirectory(Dependencies)
add_subdirectory(Src)

set_property(TARGET ComInterfaceGenerator PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(ComInterfaceGenerator 
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/generated/"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/antlr/runtime/"
)


target_sources(ComInterfaceGenerator 
	PRIVATE 
		generated/CidlBaseListener.cpp 
		generated/CidlLexer.cpp 
		generated/CidlListener.cpp
		generated/CidlParser.cpp
)

target_compile_definitions(ComInterfaceGenerator PRIVATE ANTLR4CPP_STATIC)

target_link_libraries(ComInterfaceGenerator PRIVATE CommandLine)
target_link_libraries(ComInterfaceGenerator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/antlr/lib/$<IF:$<CONFIG:Debug>,Debug,Release>/antlr4-runtime-static.lib")