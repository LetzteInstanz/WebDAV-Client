find_package(Qt6 6.8 REQUIRED COMPONENTS Core)
if (NOT Qt6_FOUND)
    message(SEND_ERROR "Failed to find Qt6 components")
    return()
endif()
qt_standard_project_setup(REQUIRES 6.8)
include(CMakePrintHelpers)
set(WDC_SRC_PATH ${WDC_PATH}/src)
cmake_print_variables(WDC_SRC_PATH)
set(FS_PATH ${WDC_SRC_PATH}/FileSystem)
cmake_print_variables(FS_PATH)
add_executable(tests
    ${COMMON_SRC}/Util.cpp
    ${COMMON_SRC}/Util.h
    ${FS_PATH}/include/FileSystem/FileSystemObject.h
    ${FS_PATH}/src/FileSystemObject.cpp
    ${FS_PATH}/src/Parser/FSObjectStruct.cpp
    ${FS_PATH}/src/Parser/FSObjectStruct.h
    ${FS_PATH}/src/Parser/Parser.cpp
    ${FS_PATH}/src/Parser/Parser.h
    ${FS_PATH}/src/Parser/TimeParser.cpp
    ${FS_PATH}/src/Parser/TimeParser.h
    ${FS_PATH}/src/ReadBuffer.h
    ${WDC_SRC_PATH}/Qml/FileItemModel/SizeDisplayer.cpp
    ${WDC_SRC_PATH}/Qml/FileItemModel/SizeDisplayer.h
    src/ParserTest.cpp
    src/ParserTest.h
    src/SizeDisplayerTest.cpp
    src/SizeDisplayerTest.h
    src/Tests.cpp
    src/Tests.h
    src/Util.cpp
    src/Util.h
    src/main.cpp
    src/pch.h
)
set_property(TARGET tests PROPERTY CXX_STANDARD 26)
target_include_directories(tests PRIVATE ${COMMON_SRC}/.. ${FS_PATH}/include ${FS_PATH}/src ${WDC_SRC_PATH})
target_link_libraries(tests PRIVATE Qt6::Core stdc++exp)
target_compile_definitions(tests PRIVATE TESTS)
target_compile_options(tests PRIVATE -fno-inline)
target_precompile_headers(tests PRIVATE src/pch.h)
install(TARGETS tests BUNDLE DESTINATION . LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
