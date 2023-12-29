cmake_minimum_required(VERSION 3.16)

project(web_dav_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(web_dav_client
    src/App.cpp
    src/App.h
    src/Json/DataJsonFile.cpp
    src/Json/DataJsonFile.h
    src/Json/JsonFile.cpp
    src/Json/JsonFile.h
    src/Json/SettingsJsonFile.cpp
    src/Json/SettingsJsonFile.h
    src/Logger.cpp
    src/Logger.h
    src/main.cpp
    src/pch.h
    src/ServerInfo.cpp
    src/ServerInfo.h
    src/ServerInfoManager.cpp
    src/ServerInfoManager.h
    src/Util.cpp
    src/Util.h
)

qt_add_qml_module(web_dav_client
    URI WebDavClient
    VERSION 1.0
    QML_FILES
        src/Qml/BorderRectangle.qml
        src/Qml/EditServerDialog.qml
        src/Qml/Main.qml
    SOURCES
        src/Qml/Logger.cpp
        src/Qml/Logger.h
        src/Qml/Settings.cpp
        src/Qml/Settings.h
        src/Qml/ServerItemModel.cpp
        src/Qml/ServerItemModel.h
)

include_directories(src/Qml) # https://doc.qt.io/qt-6/qtqml-cppintegration-definetypes.html: All headers that declare QML types need to be accessible without any prefix from the project's include path.

target_precompile_headers(web_dav_client PRIVATE src/pch.h)

set_property(TARGET web_dav_client PROPERTY CXX_STANDARD 20)

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
