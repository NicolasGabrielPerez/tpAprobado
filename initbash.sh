#!/bin/bash
cd so-commons-library
sudo make install
cd ..

cd ansisop-parser
cd parser
make all
sudo make install
cd ..
cd ..

cd push-library
make all
sudo make install
cd ..
