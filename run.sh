#!/bin/sh

cmake -B build -G "Ninja" .
cmake --build build
build/VES
