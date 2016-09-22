#!/bin/bash

# javac HelloWorld.java; 
# java Helloworld;

if g++ *.cpp -Wall -o TTT.exe;
then
   echo "Compilation successful!"
    ./TTT.exe init verbose < pipe | ./TTT.exe > pipe
else
   echo "Compilation failed."
fi