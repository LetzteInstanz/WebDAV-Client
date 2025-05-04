# WebDAVClient

WebDAVClient is a client implementation of [WebDAV](https://en.wikipedia.org/wiki/WebDAV) for Android. The application supports at the moment only file system navigation over HTTP without authentication.

Build Requirements
-------
It is necessary to install the [nlohmann/json](https://github.com/nlohmann/json) 3.11.3 package or specify a path in the `WDC_3RD_PARTY_DEPEND_PATH` environment variable to the library binaries.

The project can be built for [Android](https://doc.qt.io/qt-6.8/android.html) at least by:
* Android SDK 10–14 (API level 29–34)
* OpenJDK 21.0.5
* NDK 26.1.10909125

The project can be built for Linux as well at least by:
* Qt 6.8.2
* CMake 3.28.3
* Ninja 1.11.1
* GCC 14.2.0

Deployment
-------
It is necessary to [sign](https://forum.qt.io/post/739225) the Android release package, otherwise Android will refuse installation of the package.

License
-------
WebDAVClient uses [Ubuntu Yaru theme suite's](https://github.com/ubuntu/yaru/tree/master) icons, that are licensed under the terms of the [Creative Commons Attribution-ShareAlike 4.0 License](https://creativecommons.org/licenses/by-sa/4.0/). This program is licensed under CC BY-SA 4.0 too.
