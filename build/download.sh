#!/bin/bash

curl -L -O https://www.dropbox.com/sh/clqo7ekr0ysbrip/AACsp1F6JQVYxp2Ocvlbexf9a/drs-5.0.6.tar.gz?dl=0
mv drs-5.0.6.tar.gz\?dl=0 drs-5.0.6.tar.gz
tar xzvf drs-5.0.6.tar.gz
cp MakeFile drs-5.0.6
cp DOFrame.h drs-5.0.6/include
cd drs-5.0.6
make
