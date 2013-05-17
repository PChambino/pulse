Setup
=====

Android SDK
-----------

It is an Android application after all.


OpenCV Android SDK
------------------

Download `OpenCV-2.4.5-android-sdk`.
Copy to `./libs`, then:

    cd ./libs/OpenCV-2.4.5-android-sdk/sdk/java
    android update lib-project -p . -t android-17


Android NDK
-----------

Download `android-ndk-r8d`.
Copy to wherever you want it.


Local properties
----------------

Create or edit your `local.properties` file and add the following properties:
  - `sdk.dir` with the path to the Android SDK
  - `ndk.dir` with the path to the Android NDK


Attribution
===========

[Heart](http://thenounproject.com/noun/heart/#icon-No15259) designed by [Diego Naive](http://thenounproject.com/diegonaive) from The Noun Project
