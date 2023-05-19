#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$(dirname "$0")" ; pwd -P)"
ROOT_DIR=$SCRIPT_DIR/../

download_unzip () {
    URL=$1
    DIR=$2
    echo "downloading zip from $URL and putting in $DIR"
    rm -rf $DIR
    mkdir -p $DIR
    curl --fail -L "$URL" -o $DIR/package.zip
    unzip $DIR/package.zip -d $DIR
    rm $DIR/package.zip
}

download_tgz () {
    URL=$1
    DIR=$2
    echo "downloading tgz from $URL and putting in $DIR"
    rm -rf $DIR
    mkdir -p $DIR
    curl --fail -L "$URL" -o $DIR/package.tar.gz
    tar -zxvf $DIR/package.tar.gz -C $DIR
    rm $DIR/package.tar.gz
}

OPENCV_WIN_URL=https://devops-generic.pkg.coding.smoa.cloud/smore-vision/3rdparty/opencv-4.5.3-win64_vc16.zip?version=4.5.3
OPENCV_WIN_DIR=$ROOT_DIR/3rdparty/opencv_win

download_unzip $OPENCV_WIN_URL $OPENCV_WIN_DIR 