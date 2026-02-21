find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Qml Quick)
if (NOT Qt6_FOUND)
    message(SEND_ERROR "Failed to find Qt6 components")
    return()
endif()

include(FetchContent)
if (CMAKE_MINOR_VERSION LESS 30)
    set(FETCHCONTENT_QUIET FALSE)
endif()
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_PROGRESS TRUE
    FIND_PACKAGE_ARGS 3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

qt_standard_project_setup(REQUIRES 6.8)
add_subdirectory(src/FileSystem)
qt_add_executable(wdc
    ${COMMON_SRC}/QtUtil.cpp
    ${COMMON_SRC}/QtUtil.h
    ${COMMON_SRC}/Util.cpp
    ${COMMON_SRC}/Util.h
    src/App.cpp
    src/App.h
    src/Json/JsonFile.cpp
    src/Json/JsonFile.h
    src/Json/JsonObject.cpp
    src/Json/JsonObject.h
    src/Json/RootObject.cpp
    src/Json/RootObject.h
    src/Logger.cpp
    src/Logger.h
    src/Qml/FileSystem/AbstractFileSystemModel.cpp
    src/Qml/FileSystem/AbstractFileSystemModel.h
    src/Qml/FileSystem/DownloadFileSystemModel.cpp
    src/Qml/FileSystem/DownloadFileSystemModel.h
    src/Qml/FileSystem/FileSystemInfo.cpp
    src/Qml/FileSystem/FileSystemInfo.h
    src/Qml/FileSystem/MainFileSystemModel.cpp
    src/Qml/FileSystem/MainFileSystemModel.h
    src/Qml/Factory/FileSystemModelFactory.cpp
    src/Qml/Factory/FileSystemModelFactory.h
    src/Qml/Factory/LogItemModelFactory.cpp
    src/Qml/Factory/LogItemModelFactory.h
    src/Qml/Factory/ServerItemModelFactory.cpp
    src/Qml/Factory/ServerItemModelFactory.h
    src/Qml/Factory/SortParamItemModelFactory.cpp
    src/Qml/Factory/SortParamItemModelFactory.h
    src/Qml/FileItemModel/FileItemModel.cpp
    src/Qml/FileItemModel/FileItemModel.h
    src/Qml/FileItemModel/Role.h
    src/Qml/FileItemModel/SizeDisplayer.cpp
    src/Qml/FileItemModel/SizeDisplayer.h
    src/Qml/IconProvider.cpp
    src/Qml/IconProvider.h
    src/Qml/Logger/LogItemModel.cpp
    src/Qml/Logger/LogItemModel.h
    src/Qml/Settings.cpp
    src/Qml/Settings.h
    src/Qml/ServerItemModel.cpp
    src/Qml/ServerItemModel.h
    src/Qml/Sort/FileSortFilterItemModel.cpp
    src/Qml/Sort/FileSortFilterItemModel.h
    src/Qml/Sort/SortParam.cpp
    src/Qml/Sort/SortParam.h
    src/Qml/Sort/SortParamItemModel.cpp
    src/Qml/Sort/SortParamItemModel.h
    src/ServerInfo.cpp
    src/ServerInfo.h
    src/ServerInfoManager/RootSection.cpp
    src/ServerInfoManager/RootSection.h
    src/ServerInfoManager/ServerInfoManager.cpp
    src/ServerInfoManager/ServerInfoManager.h
    src/Settings/DownloadSection.cpp
    src/Settings/DownloadSection.h
    src/Settings/RootSection.cpp
    src/Settings/RootSection.h
    src/Settings/Settings.cpp
    src/Settings/Settings.h
    src/main.cpp
    src/pch.h
)
set_target_properties(wdc
    PROPERTIES
        CXX_STANDARD 26
        QT_ANDROID_PACKAGE_SOURCE_DIR ${WDC_PATH}/android
)

if (ANDROID)
    find_package(Qt6 6.8 REQUIRED COMPONENTS CorePrivate)
    if (NOT Qt6_FOUND)
        message(SEND_ERROR "Failed to find CorePrivate component in Qt6 package")
        return()
    endif()
    target_link_libraries(wdc PRIVATE Qt6::CorePrivate)
    target_sources(wdc
        PRIVATE
            android/src/org/qtproject/webdavclient/NotificationClient.java
            android/src/org/qtproject/webdavclient/MyBroadcastReceiver.java
            src/NotificationClient.cpp
            src/NotificationClient.h
    )
endif()

qt_add_qml_module(wdc
    URI WebDavClient
    VERSION 1.0
    QML_FILES
        src/Qml/ConfirmationDialog.qml
        src/Qml/Core/BorderRectangle.qml
        src/Qml/Core/Button.qml
        src/Qml/Core/ContentItem.qml
        src/Qml/Core/ListView.qml
        src/Qml/Core/MessageBox.qml
        src/Qml/Core/SelectionSequentialAnimation.qml
        src/Qml/Core/Timer.qml
        src/Qml/EditServerDialog.qml
        src/Qml/FileItemMenu.qml
        src/Qml/FileListPageColumnLayout.qml
        src/Qml/Logger/LogItemMenu.qml
        src/Qml/Logger/LogPageColumnLayout.qml
        src/Qml/Main.qml
        src/Qml/ProgressDialog.qml
        src/Qml/ServerItemMenu.qml
        src/Qml/ServerListPageColumnLayout.qml
        src/Qml/SettingsPageColumnLayout.qml
        src/Qml/Sort/SortDialog.qml
        src/Qml/Util.js
#    RESOURCES
#        android/AndroidManifest.xml
#        android/build.gradle
#        android/res/values/libs.xml
#        android/res/xml/qtprovider_paths.xml
#        android/gradle/wrapper/gradle-wrapper.jar
#        android/gradle/wrapper/gradle-wrapper.properties
#        android/gradle.properties
#        android/gradlew
#        android/gradlew.bat
)

qt_add_resources(wdc "images"
    PREFIX "/"
    FILES
        res/icons/application-apk.png
        res/icons/application-epub+zip.png
        res/icons/application-geo+json.png
        res/icons/application-illustrator.png
        res/icons/application-json.png
        res/icons/application-msexcel.png
        res/icons/application-octet-stream.png
        res/icons/application-pdf.png
        res/icons/application-pgp-encrypted.png
        res/icons/application-pgp-keys.png
        res/icons/application-pgp-signature.png
        res/icons/application-photoshop.png
        res/icons/application-postscript.png
        res/icons/application-rss+xml.png
        res/icons/application-vnd-dart.png
        res/icons/application-vnd.debian.binary-package.png
        res/icons/application-vnd.flatpak.png
        res/icons/application-vnd.iccprofile.png
        res/icons/application-vnd.ms-access.png
        res/icons/application-vnd.ms-powerpoint.png
        res/icons/application-vnd.ms-publisher.png
        res/icons/application-vnd.ms-word.png
        res/icons/application-vnd.oasis.opendocument.drawing.png
        res/icons/application-vnd.openofficeorg.extension.png
        res/icons/application-vnd.snap.png
        res/icons/application-vnd.squashfs.png
        res/icons/application-x-7z-compressed.png
        res/icons/application-x-apple-diskimage.png
        res/icons/application-x-audacity-project.png
        res/icons/application-x-bittorrent.png
        res/icons/application-x-cd-image.png
        res/icons/application-x-desktop.png
        res/icons/application-x-executable.png
        res/icons/application-x-firmware.png
        res/icons/application-x-gzip.png
        res/icons/application-x-hwp.png
        res/icons/application-x-ipynb+json.png
        res/icons/application-x-java.png
        res/icons/application-x-karbon.png
        res/icons/application-x-krita.png
        res/icons/application-x-lmms-project.png
        res/icons/application-x-mobipocket-ebook.png
        res/icons/application-x-model.png
        res/icons/application-x-ms-dos-executable.png
        res/icons/application-x-mswinurl.png
        res/icons/application-x-musescore3.png
        res/icons/application-x-perl.png
        res/icons/application-x-php.png
        res/icons/application-x-pkcs7-certificates.png
        res/icons/application-x-python-bytecode.png
        res/icons/application-x-qemu-disk.png
        res/icons/application-x-rar.png
        res/icons/application-x-raw-disk-image.png
        res/icons/application-x-shellscript.png
        res/icons/application-x-subrip.png
        res/icons/application-x-theme.png
        res/icons/application-x-virtualbox-hdd.png
        res/icons/application-x-virtualbox-ova.png
        res/icons/application-x-virtualbox-ovf.png
        res/icons/application-x-virtualbox-vbox-extpack.png
        res/icons/application-x-virtualbox-vbox.png
        res/icons/application-x-virtualbox-vdi.png
        res/icons/application-x-virtualbox-vhd.png
        res/icons/application-x-virtualbox-vmdk.png
        res/icons/application-x-yaml.png
        res/icons/application-x-zip.png
        res/icons/audio-midi.png
        res/icons/audio-x-flac.png
        res/icons/audio-x-generic.png
        res/icons/audio-x-mpeg.png
        res/icons/audio-x-mpegurl.png
        res/icons/audio-x-ms-wma.png
        res/icons/audio-x-vorbis+ogg.png
        res/icons/audio-x-wav.png
        res/icons/folder.png
        res/icons/font-x-generic.png
        res/icons/html-template.png
        res/icons/image-svg+xml.png
        res/icons/image-x-cursor.png
        res/icons/image-x-generic.png
        res/icons/image-x-xcf.png
        res/icons/libreoffice-oasis-database.png
        res/icons/libreoffice-oasis-formula.png
        res/icons/libreoffice-oasis-master-document.png
        res/icons/libreoffice-oasis-presentation.png
        res/icons/libreoffice-oasis-spreadsheet.png
        res/icons/libreoffice-oasis-text.png
        res/icons/message-rfc822.png
        res/icons/package-x-generic.png
        res/icons/rom.png
        res/icons/shellscript.png
        res/icons/text-css.png
        res/icons/text-dockerfile.png
        res/icons/text-html.png
        res/icons/text-less.png
        res/icons/text-markdown.png
        res/icons/text-richtext.png
        res/icons/text-rust.png
        res/icons/text-x-arduino.png
        res/icons/text-x-c++hdr.png
        res/icons/text-x-chdr.png
        res/icons/text-x-cobol.png
        res/icons/text-x-copying.png
        res/icons/text-x-c.png
        res/icons/text-x-cpp.png
        res/icons/text-x-csharp.png
        res/icons/text-x-fortran.png
        res/icons/text-x.gcode.png
        res/icons/text-x-generic.png
        res/icons/text-x-gettext-translation.png
        res/icons/text-x-go.png
        res/icons/text-x-haskell.png
        res/icons/text-x-install.png
        res/icons/text-x-javascript.png
        res/icons/text-x-lilypond.png
        res/icons/text-x-log.png
        res/icons/text-x-lua.png
        res/icons/text-x-makefile.png
        res/icons/text-xml.png
        res/icons/text-x-nim.png
        res/icons/text-x-patch.png
        res/icons/text-x-python.png
        res/icons/text-x-qml.png
        res/icons/text-x-readme.png
        res/icons/text-x-r.png
        res/icons/text-x-ruby.png
        res/icons/text-x-sass.png
        res/icons/text-x-scala.png
        res/icons/text-x-script.png
        res/icons/text-x-sql.png
        res/icons/text-x-systemd-unit.png
        res/icons/text-x-tex.png
        res/icons/text-x-typescript.png
        res/icons/text-x-vala.png
        res/icons/unknown.png
        res/icons/video-x-generic.png
)
target_include_directories(wdc PRIVATE ${COMMON_SRC}/.. src/Qml) # note: https://doc.qt.io/qt-6/qtqml-cppintegration-definetypes.html: All headers that declare QML types need to be accessible without any prefix from the project's include path.
target_link_libraries(wdc PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick nlohmann_json::nlohmann_json file_system)
target_precompile_headers(wdc PRIVATE src/pch.h)
install(TARGETS wdc BUNDLE DESTINATION . LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
