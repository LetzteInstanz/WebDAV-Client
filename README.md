# WebDAVClient

WebDAVClient is a client implementation of [WebDAV](https://en.wikipedia.org/wiki/WebDAV) for Android. The application supports at the moment only file system navigation over HTTP without authentication.

Build Requirements
-------

The project can be built for Linux at least by:
* Qt 6.6.1
* CMake 3.22.1
* Ninja 1.10.1
* GCC 12.3.0

The project can be built for [Android](https://doc.qt.io/qt-6/android.html) as well at least by:
* Android SDK 13 (API level 33)
* OpenJDK 19.0.2
* NDK 25.1.8937393

Deployment
-------
It is necessary to [sign](https://forum.qt.io/post/739225) the Android release package, otherwise Android will refuse installation of the package.

License
-------
WebDAVClient uses [Ubuntu Yaru theme suite's](https://github.com/ubuntu/yaru/tree/master) icons, that are licensed under the terms of the [Creative Commons Attribution-ShareAlike 4.0 License](https://creativecommons.org/licenses/by-sa/4.0/). This program is licensed under CC BY-SA 4.0 too.
