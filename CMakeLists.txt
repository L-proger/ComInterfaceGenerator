cmake_minimum_required (VERSION 3.15.0)

project(ComInterfaceGeneratorProj LANGUAGES CXX)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_link_options(-static-libgcc)
	add_link_options(-static-libstdc++)
endif()

set(ANTLR_BUILD_CPP_TESTS OFF)

add_executable(ComInterfaceGenerator)

set_property(TARGET ComInterfaceGenerator PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET ComInterfaceGenerator PROPERTY CXX_STANDARD 17)
add_subdirectory(Dependencies)
add_subdirectory(Src)

target_link_libraries(ComInterfaceGenerator PRIVATE CommandLine)
target_link_libraries(ComInterfaceGenerator PRIVATE libComInterfaceGenerator)


function(ComInterfaceGeneratorMakeArgs)
    set(oneValueArgs RESULT_VAR)
    set(multiValueArgs OUTMODULES)
    cmake_parse_arguments(MakeGeneratorArgs "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(RESULT "")

    if(ApiEnableExceptions)
        list(PREPEND RESULT -e true)
    else()
        list(PREPEND RESULT -e false)
    endif()

    list(PREPEND RESULT -l ${ApiOutputLanguage})

    foreach(INC ${ApiIncludeDirs})
        list(APPEND RESULT -I ${INC})
    endforeach()
    
    list(APPEND RESULT -o ${ApiOutputDir})
    list(APPEND RESULT ${MakeGeneratorArgs_OUTMODULES})
    set(${MakeGeneratorArgs_RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()


function(ComInterfaceGeneratorBaseArgs)
    set(oneValueArgs RESULT_VAR)
    cmake_parse_arguments(MakeGeneratorArgs2 "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(RESULT "")

    if(ApiEnableExceptions)
        list(PREPEND RESULT -e true)
    else()
        list(PREPEND RESULT -e false)
    endif()

    list(PREPEND RESULT -l ${ApiOutputLanguage})

    foreach(INC ${ApiIncludeDirs})
        list(APPEND RESULT -I ${INC})
    endforeach()
    
    list(APPEND RESULT -o ${ApiOutputDir})

    set(${MakeGeneratorArgs2_RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()