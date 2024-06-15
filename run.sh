#!/bin/bash
cd build
cmake .. && make
cd ..
build/compiler -ast example/hello.c 
