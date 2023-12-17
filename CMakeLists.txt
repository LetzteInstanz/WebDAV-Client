cmake_minimum_required(VERSION 3.16)

project(web_dav_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(web_dav_client
    App.cpp
    App.h
    main.cpp
    pch.h
    ServerInfo.cpp
    ServerInfo.h
    ServerInfoManager.cpp
    ServerInfoManager.h
    ServerItemModel.cpp
    ServerItemModel.h
    Settings.cpp
    Settings.h
    Util.cpp
    Util.h
)

qt_add_qml_module(web_dav_client
    URI WebDavClient
    VERSION 1.0
    QML_FILES
        BorderRectangle.qml
        Main.qml
        EditServerDialog.qml
)

target_precompile_headers(web_dav_client PRIVATE pch.h)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(web_dav_client PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.web_dav_client
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(web_dav_client
    PRIVATE
        Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS web_dav_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
