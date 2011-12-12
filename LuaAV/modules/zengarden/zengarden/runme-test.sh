#!/bin/bash
 
java -Djava.library.path=./libs/`./src/platform`/ -classpath ./ZenGarden.jar:./junit-4.8.2.jar org.junit.runner.JUnitCore me.rjdj.zengarden.PdObjectTest
