#!/bin/bash

# javac HelloWorld.java; 
# java Helloworld;

if g++ *.cpp -Wall -std=c++11 -o checkers.exe;
then
   echo "Compilation successful!"
    ./checkers.exe init verbose < pipe | ./checkers.exe > pipe
else
   echo "Compilation failed."
fi