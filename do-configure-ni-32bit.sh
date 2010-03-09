#!/bin/bash

# This script is only useful for members of the AG neuroinformatics at uni bielefeld

./waf configure --with-xercesc-dir=/vol/xcf --with-xsd-include-dir=$HOME/src/build/xsd-3.2.0+dep/install/include/ --with-xsd=/homes/fschmidt/src/build/xsd-3.2.0+dep/install/bin/xsd  --with-kdl-dir=/vol/orocos --with-eigen2-include-dir=/vol/numerik/ --with-kdl-chain-viz-dir=/homes/fschmidt/local --prefix=/vol/nirobots/ --with-pa10-dir=/vol/nirobots --with-spacenavi-dir=/vol/nirobots/  $*
