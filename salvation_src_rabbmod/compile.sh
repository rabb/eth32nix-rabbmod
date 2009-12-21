#!/bin/sh

scons && scons pk3 && mkdir -p dll && cd client && scons && cp cgame.mp.i386.so ../dll && cd ..
