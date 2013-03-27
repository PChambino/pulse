Setup
=====

OpenCV Android SDK
------------------

Download `OpenCV-2.4.4-android-sdk`.
Copy to `./libs`, then:

    cd ./libs/OpenCV-2.4.4-android-sdk/sdk/java
    android update lib-project -p . -t android-17
    patch -p3 < ../../../OpenCV-2.4.4-android-sdk.patch

Android NDK
-----------

Download `android-ndk-r8d`.
Copy to wherever you want it.

Deploy
======

    <android-ndk-path>/ndk-build && ant debug install
