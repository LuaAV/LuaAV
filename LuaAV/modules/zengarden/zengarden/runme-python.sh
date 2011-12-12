#!/bin/sh
 
if [ "$1" == "" ]
then
	patch=pd-patches/simple_osc.pd
else
	patch=$1
fi
 
LD_LIBRARY_PATH=./libs/`./src/platform`/ DYLD_LIBRARY_PATH=./libs/`./src/platform`/ python pyExampleGarden.py $patch
