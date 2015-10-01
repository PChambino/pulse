pulse
=====

### Android-based implementation of Eulerian Video Magnification for vital signs monitoring

http://p.chambino.com/dissertation

Eulerian Video Magnification is a recently presented method capable of
revealing temporal variations in videos that are impossible to see with
the naked eye. Using this method, it is possible to visualize the flow of
blood as it fills the face. From its result, a person’s heart rate is possible
to be extracted.

This research work was developed at Fraunhofer Portugal and its goal is to
test the feasibility of the implementation of the Eulerian Video Magnification
method on smartphones by developing an Android application for monitoring
vital signs based on the Eulerian Video Magnification method.

There has been some successful effort on the assessment of vital signs,
such as, heart rate, and breathing rate, in a contact-free way using a
webcamera and even a smartphone. However, since the Eulerian Video
Magnification method was recently proposed, its implementation has not been
tested in smartphones yet.Thus, the Eulerian Video Magnification method
performance for color amplification was optimized in order to execute on an
Android device at a reasonable speed.

The Android application implemented includes features, such as, detection
of a person’s cardiac pulse, dealing with artifacts’ motion, and real-time
display of the magnified blood flow. Then, the application measurements were
evaluated through tests with several individuals and compared to the ones
detected by the ViTrox application and to the readings of a sphygmomanometer.


Dependencies
============

* Android SDK (v17)
* Android NDK (r10e)
* OpenCV Android SDK (2.4.11)
* [pulse-cpp]

[pulse-cpp]: https://github.com/pchambino/pulse-cpp


Setup
=====

    cd pulse
    android update project -p . -t android-17
    # a local.properties file with sdk.dir should have been created
    echo ndk.dir=PATH_TO_ANDROID_NDK >> local.properties
    echo pulse-cpp.dir=PATH_TO_PULSE_CPP >> local.properties
    # copy OpenCV Android SDK to libs directory
    android update lib-project -p libs/OpenCV-android-sdk/sdk/java -t android-17
    echo android.library=true >> project.properties
    ant debug install


Attribution
===========

[Heart] designed by [Diego Naive] from The Noun Project

[Heart]: http://thenounproject.com/noun/heart/#icon-No15259
[Diego Naive]: http://thenounproject.com/diegonaive
