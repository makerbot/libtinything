libtinything
============

functions to create, modify and validate tinything (to be named) files

Building
========

The libtinything sconsript currently allows it to be built on a local ubuntu system or cross compiled for the birdwing board.

To install to local /usr/lib run:
scons 

To cross compile and install to the /usr/lib of a NFS run:
scons --embedded --nfs=/path/to/nfs/root
