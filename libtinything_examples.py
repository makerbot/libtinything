# here is an example of extracting a jsontoolpath
from ctypes import *
libtinything = CDLL("/home/paul/libtinything/lib/libtinything.so")
tinything = libtinything.NewTinyThing("/home/paul/libtinything/davesthing.tinything")
libtinything.UnzipToolpath(tinything) # 1 = success, 0 = failed
libtinything.GetToolpath.restype=c_char_p
libtinything.GetToolpath(tinything)

