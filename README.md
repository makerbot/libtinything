libtinything
============

Functions to create, read and validate .makerbot files

The spec for .makerbot files is here: https://github.com/makerbot/Birdwing-toolchain/blob/master/docs/architecture/tiny_thing.md

Tinything?
==========

This was our internal name for what are now called .makerbot files

Python Bindings
===============

Libtinything comes with python bindings! Here are some potentially out of date examples.

Here is an example of extracting a jsontoolpath using the python bindings using 
PYTHON3.3

    import ctypes
    libtt = ctypes.CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThingReader(ctypes.c_char_p(bytes("/home/paul/test.tinything", 'UTF-8')))
    libtt.UnzipToolpath(tt) # 1 = success, 0 = failed
    libtt.GetToolpath.restype=c_char_p
    libtt.GetToolpath(tinything)


Here is an example of incrementally reading a toolpath

    import ctypes
    libtt = ctypes.CDLL("/home/paul/libtinything/lib/libtinything.so")
    tt = libtt.NewTinyThingReader(ctypes.c_char_p(bytes("/home/paul/test.tinything", 'UTF-8')))
    libtt.GetToolpathIncr.restype=c_char_p
    chars = libtt.GetToolpathIncr(tt, ctypes.c_int(10)) # get the first 10 characters
    chars = libtt.GetToolpathIncr(tt, ctypes.c_int(20)) # get the next 20..

Here is an example of creating a tinything with python 2.7

    import ctypes
    libtt = ctypes.CDLL("/home/paul/libtinything/obj/libtinything.so")
    tt = libtt.NewTinyThingWriter(ctypes.c_char_p("/home/paul/test.tinything"))
    libtt.SetMetadataFile(tt, ctypes.c_char_p("/home/paul/metadata.txt"))
    libtt.SetToolpathFile(tt, ctypes.c_char_p("/home/paul/toolpath.txt"))
    libtt.SetThumbnailDirectory(tt, ctypes.c_char_p("/home/paul/makerbots/thumbnails"))
    libtt.Zip(tt)
