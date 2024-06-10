#!/bin/bash
cd build
cmake .. && make
cd ..
build/compiler example/hello.c -o hello.sy