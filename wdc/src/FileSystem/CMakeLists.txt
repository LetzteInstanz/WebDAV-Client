find_package(Qt6 6.8 REQUIRED COMPONENTS Core Network)
if (NOT Qt6_FOUND)
    message(SEND_ERROR "Failed to find Qt6 components")
    return()
endif()

qt_add_library(file_system STATIC
    ${COMMON_SRC}/QtUtil.cpp
    ${COMMON_SRC}/QtUtil.h
    ${COMMON_SRC}/Util.cpp
    ${COMMON_SRC}/Util.h
    include/FileSystem/FileSystemModel.h
    include/FileSystem/FileSystemObject.h
    src/Client/Client.cpp
    src/Client/Client.h
    src/FileSystemModel.cpp
    src/FileSystemObject.cpp
    src/Parser/FSObjectStruct.cpp
    src/Parser/FSObjectStruct.h
    src/Parser/Parser.cpp
    src/Parser/Parser.h
    src/ReadBuffer.h
    src/Parser/TimeParser.cpp
    src/Parser/TimeParser.h
    src/ThreadData.cpp
    src/ThreadData.h
    src/pch.h
)
set_property(TARGET file_system PROPERTY CXX_STANDARD 26)
target_include_directories(file_system PRIVATE ${COMMON_SRC}/.. PUBLIC include)
target_link_libraries(file_system PUBLIC Qt6::Network PRIVATE Qt6::Core)
target_precompile_headers(file_system PRIVATE src/pch.h)
install(TARGETS file_system BUNDLE DESTINATION . LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
