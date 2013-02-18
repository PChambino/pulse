#!/usr/bin/env bash

mvn install:install-file -Dfile=opencv-2.4.3.jar -DgroupId=org.opencv -DartifactId=opencv -Dpackaging=jar -Dversion=2.4.3.2
