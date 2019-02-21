LibTinyThing: C++ library with Python bindings for reading and writing .makerbot/.tinything files

# Why tinything
Don't worry about it

# Makerbot File Spec
.makerbot files are the files that are sent to birdwing bots. They are a zip file with a json toolpath (see separate spec), metadata, and thumbnails.

## Versioning
.makerbot files have a version number, which is contained in the meta.json file. This version number follows [semantic versioning](http://semver.org/).
Version 1.0.0 is the first version with this version number. Any meta.json file that does not have a "version" key is by definition a version 0.0.3 file.
Version 2.0.0 allows for comment commands in JsonToolPath files.
Version 3.0.0 moves to a multi-extruder-by-default scheme for the meta file. Any key in the metafile that can be different on a per-extruder basis should always be a list, with a number of entries equivalent to the number of extruders on the machine - NOT the number of extruders used in a print.

## JsonToolpath File
filename: "print.jsontoolpath"

Describes the toolpath. See [the spec](jsontoolpath.md) in this directory for more information regarding this file.  This file can be reduced to a gcode file.

The jsontoolpath file is output by Miracle Grue.

## Thumbnails
.makerbot files contain 6 thumbnails.
All bots have the 3 following:
    isometric_thumbnail_120x120
    isometric_thumbnail_320x320
    isometric_thumbnail_640x640

The other 3 thumbnails differ by bot.
Birdwing bots additionally have:

    thumbnail_55x40.png
    thumbnail_110x80.png
    thumbnail_320x200.png

Method bots additionally have:

    thumbnail_140x106.png
    thumbnail_212x300.png
    thumbnail_960x1460.png

The thumbnails will be displayed in the Bot LCD UI and will also be used in Makerware in any "tinything" file lists

Thumbnails can be any image files matching these dimensions. MakerWare currently can generate .stl and .thing renders as thumbnail images.

## Meta Data
filename: meta.json

Contains meta information about the file. See [the spec](metafile.md) in this directory for more information about this file.

