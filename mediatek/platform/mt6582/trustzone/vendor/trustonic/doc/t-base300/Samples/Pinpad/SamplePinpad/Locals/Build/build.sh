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

if [ "$MODE" == "Release" ]; then
  echo -e "Mode\t\t: Release"
  OPTIM=release
else
  echo -e "Mode\t\t: Debug"
  OPTIM=debug
fi

cd $cur_dir/../Code/Android/app

mkdir -p libs/armeabi
cp -f ${COMP_PATH_TlcSamplePinpad}/Bin/$MODE/libTlcPinpad.so libs/armeabi

ant $OPTIM

mkdir -p $cur_dir/../../Out/Bin/${MODE}
cp -f $cur_dir/../Code/Android/app/out/*.apk $cur_dir/../../Out/Bin/$MODE/
