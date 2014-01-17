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

Add ` install` to the end of either scons call to copy the compiled library to the appropriate directory.

Note: zlib is required to be present as a sibling directory to libtinything

Python Bindings
===============

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
