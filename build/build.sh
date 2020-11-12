#!/bin/bash

curl -L -O https://www.dropbox.com/sh/clqo7ekr0ysbrip/AACsp1F6JQVYxp2Ocvlbexf9a/drs-5.0.6.tar.gz?dl=0
mv drs-5.0.6.tar.gz\?dl=0 drs-5.0.6.tar.gz
tar xzvf drs-5.0.6.tar.gz
mv drs-5.0.6 daq
cp source/MakeFile daq
cp source/DOFrame.h daq/include

cp source/cmdline.h daq/include
cp source/drscmd.cpp daq/src

cd daq
make

cd ..
mv daq ../
echo Installed Successfully
