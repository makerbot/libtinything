libtinything
============

functions to create, modify and validate tinything (to be named) files

Building
========

The libtinything sconsript currently allows it to be built on a local ubuntu system or cross compiled for the birdwing board.

To install to local /usr/lib run:
`scons `

To cross compile and install to the /usr/lib of a NFS run:
`scons --embedded --nfs=/path/to/nfs/root`

Run `scons install` to copy the compiled library to the appropriate directory.

Note: zlib is required to be present as a sibling directory to libtinything

Python Bindings
===============

Here is an example of extracting a jsontoolpath using the python bindings

    from ctypes import *
    libtt = CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThingReader("/home/paul/test.tinything")
    libtt.UnzipToolpath(tinything) # 1 = success, 0 = failed
    libtt.GetToolpath.restype=c_char_p
    libtt.GetToolpath(tinything)


Here is an example of incrementally reading a toolpath

    from ctypes import *
    libtt = CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThingReader("/home/paul/test.tinything")   
    libtinything.GetToolpathIncr.restype=c_char_p
    libtinything.GetToolpathIncr(tt,10) # get the first 10 characters
    libtinything.GetToolpathIncr(tt,20) # get the next 20..

Here is an example of creating a tinything

    from ctypes import *
    libtt = CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThingWriter("/home/paul/test.tinything")
    libtt.SetMetadataFile(tt, "/home/paul/meta.txt")
    libtt.SetThumbnailFile(tt, "/home/paul/Firefox_wallpaper.png")
    libtt.SetToolpathFile(tt, "/home/paul/toolpath.json")
    libtt.Zip(tt)
