#!/usr/bin/env bash

TARGET=../pulse-android/jni/
FILES="Pulse.[ch]pp EvmGdownIIR.[ch]pp ext profiler"

cd "$(dirname "${BASH_SOURCE[0]}")/src"
cp -R $FILES "../$TARGET"

