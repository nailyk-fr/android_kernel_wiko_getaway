#!/bin/bash

# Source setup.sh
setup_script=""
cur_dir=$(dirname $(readlink -f ${BASH_SOURCE[0]}))
search_dir=$cur_dir

while [[ ${setup_script} == "" ]]
do
  setup_script=$( find $search_dir -name "setup.sh" )
  search_dir=$(dirname $search_dir)
done
if [[ ${setup_script} == "" ]]; then
  echo "ERROR: setup.sh not found"
  exit 1
fi
source $setup_script
t_sdk_root=$( dirname $setup_script )

if [ "$MODE" == "Release" ]; then
  echo -e "Mode\t\t: Release"
  OUT_DIR=Out/Bin/Release
  OPTIM=release
else
  echo -e "Mode\t\t: Debug"
  OUT_DIR=Out/Bin/Debug
  OPTIM=debug
fi

# go to project root
cd $cur_dir/../..

### ---------------- Generic Build Command ----------------

# run NDK build
${NDK_BUILD} \
    -B \
    NDK_DEBUG=1 \
    NDK_PROJECT_PATH=Locals/Code \
    NDK_APPLICATION_MK=Locals/Code/Application.mk \
    NDK_MODULE_PATH=${t_sdk_root} \
    NDK_APP_OUT=Out/_build \
    APP_BUILD_SCRIPT=Locals/Code/Android.mk \
    APP_OPTIM=$OPTIM

mkdir -p $OUT_DIR

cp -r $PWD/Out/_build/local/armeabi/* $OUT_DIR

echo
echo Output directory of build is $PWD/$OUT_DIR


echo "Creating jar file..."
JAVA_LIB_DIR=$cur_dir/../Code/Java

javac ${JAVA_LIB_DIR}/com/trustonic/tsdk/sample/TlcSampleRot13.java

jar \
    cvf $cur_dir/../../Out/Bin/$MODE/samplerot13.jar \
    -C ${JAVA_LIB_DIR}/ \
    com/trustonic/tsdk/sample/TlcSampleRot13.class

# copy binary
cp  Locals/Code/tlcSampleRot13.h $cur_dir/../../Out/Bin/$MODE/
