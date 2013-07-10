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

Python Bindings
===============

Here is an example of extracting a jsontoolpath using the python bindings

    from ctypes import *
    libtt = CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThing("/home/paul/test.tinything")
    libtt.UnzipToolpath(tinything) # 1 = success, 0 = failed
    libtt.GetToolpath.restype=c_char_p
    libtt.GetToolpath(tinything)

Here is an example of creating a jsontoolpath using the python bindings

    from ctypes import *
    libtt = CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThing("/home/paul/test.tinything")
    libtt.AddMetadata(tt, "/home/paul/meta.json")
    libtt.AddThumbnail(tt, "/home/paul/Firefox_wallpaper.png")
    libtt.AddToolpath(tt, "/home/paul/meta.json")


Here is an example of incrementally reading a toolpath

    from ctypes import *
    libtt = CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThing("/home/paul/test.tinything")   
    libtinything.GetToolpathIncr.restype=c_char_p
    libtinything.GetToolpathIncr(tt,10)
    libtinything.GetToolpathIncr(tt,20)
