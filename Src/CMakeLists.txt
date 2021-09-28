

file(GLOB_RECURSE SRC_FILES    
    "*.cpp"
)

file(GLOB_RECURSE HEADER_FILES    
    "*.h"
)

target_sources(ComInterfaceGenerator PRIVATE ${SRC_FILES} ${HEADER_FILES})

target_include_directories(ComInterfaceGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})