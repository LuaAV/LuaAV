#!/bin/bash
 
if [ "$1" == "" ]
then
	patch=pd-patches/simple_osc.pd
else
	patch=$1
fi
 
java -Djava.library.path=./libs/`./src/platform`/ -jar ZenGarden.jar $patch

