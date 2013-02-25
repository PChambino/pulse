Setup
=====

Download, build and install `opencv-2.4.4`.
Install `opencv-2.4.4.jar` to Maven repository using:

    mvn install:install-file -Dfile=PATH_TO_OPENCV_BIN/opencv-2.4.4.jar -DgroupId=org.opencv -DartifactId=opencv -Dpackaging=jar -Dversion=2.4.4

Create a Maven user's [`settings.xml`](http://maven.apache.org/settings.html) 
file with something like:

    <?xml version="1.0" encoding="UTF-8"?>
    <settings xmlns="http://maven.apache.org/SETTINGS/1.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
              xsi:schemaLocation="http://maven.apache.org/SETTINGS/1.0.0 http://maven.apache.org/xsd/settings-1.0.0.xsd">

        <profiles>
            <profile>
                <activation>
                    <activeByDefault>true</activeByDefault>
                </activation>
                <properties>
                    <opencv-lib.path>PATH_TO_OPENCV_LIB</opencv-lib.path>
                </properties>
            </profile>
        </profiles>

    </settings>

Finally, install the `opencv_java` library if it was not installed by `make install`.
