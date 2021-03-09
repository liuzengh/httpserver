#!/bin/sh


set -o xtrace

SOURCE_DIR=`pwd`
BUILD_DIR="build"

mkdir -p $BUILD_DIR\
    && cd $BUILD_DIR \
    && cmake $SOURCE_DIR \
    && make $*