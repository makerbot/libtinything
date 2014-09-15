import ctypes
import os
import sys
import shutil

class TinyThingTest():
    def __init__(self):
        self._toolpath = self.generate_toolpath()
        self._basepath = os.path.dirname(__file__)
        self._filepath = "tmp.makerbot"
        self._lib = self.load_library()

    def run(self):
        self.make_tinything()
        self.read_tinything()
        self.read_tinything_incr()
        self.read_tinything_fd()

    def load_library(self):
        # Hack because lib won't load outside current dir for some reason
        filename = "libtinything.so"
        libpath = os.path.abspath(os.path.join(self._basepath, "..", "obj", filename))
        print("library exists: {0}".format(os.path.isfile(libpath)))
        #shutil.copyfile(libpath, filename)
        return ctypes.CDLL(libpath)

    def generate_toolpath(self):
        # Don't try to print this toolpath
        return "".join([str(n) for n in range(50)])

    def make_tinything(self):
        print("Make file")
        tmp_toolpath = "tmp.jsontoolpath"
        writer = self._lib.NewTinyThingWriter(ctypes.c_char_p(bytes(self._filepath, "UTF-8")))
        with open(tmp_toolpath, "w") as tp:
            tp.write(self._toolpath)
        self._lib.SetToolpathFile(writer, ctypes.c_char_p(bytes(tmp_toolpath, "UTF-8")))
        if not self._lib.Zip(writer):
            print("Failed to write tinything")
        os.remove(tmp_toolpath)

    def read_tinything(self):
        print("Read file")
        reader = self._lib.NewTinyThingReader(ctypes.c_char_p(bytes(self._filepath, "UTF-8")))
        if not self._lib.UnzipToolpath(reader):
            print("Cannot unzip toolpath")
        self._lib.GetToolpath.restype = ctypes.c_char_p
        toolpath = self._lib.GetToolpath(reader)
        print(toolpath)
        if toolpath.decode("UTF-8") != self._toolpath:
            print("Toolpaths do not match")

    def read_tinything_incr(self):
        print("Read file incrementally")
        reader = self._lib.NewTinyThingReader(ctypes.c_char_p(bytes(self._filepath, "UTF-8")))
        if not self._lib.UnzipToolpath(reader):
            print("Cannot unzip toolpath")
        self._lib.GetToolpathIncr.restype = ctypes.c_char_p
        chars = self._lib.GetToolpathIncr(reader, ctypes.c_int(10)) # get the first 10 characters
        print(chars)
        if chars.decode("UTF-8") != self._toolpath[:10]:
            print("Toolpath increment 10 fail")
        chars = self._lib.GetToolpathIncr(reader, ctypes.c_int(10)) # get 10 more
        print(chars)
        if chars.decode("UTF-8") != self._toolpath[10:20]:
            print("Toolpath increment 10 fail")
        chars = self._lib.GetToolpathIncr(reader, ctypes.c_int(20)) # get 20
        print(chars)
        if chars.decode("UTF-8") != self._toolpath[20:40]:
            print("Toolpath increment 20 fail")
        chars = self._lib.GetToolpathIncr(reader, ctypes.c_int(1000)) # get the rest
        print(chars)
        if chars.decode("UTF-8") != self._toolpath[40:]:
            print("Toolpath last increment fail")
        chars = self._lib.GetToolpathIncr(reader, ctypes.c_int(1000)) # try to read more
        print(chars)
        if chars.decode("UTF-8") != "":
            print("Still getting toolpath when it should be done")

    def read_tinything_fd(self):
        print("Read file with descriptor")
        with open(self._filepath, "r") as tt:
            os.remove(self._filepath)
            print("File exists: {0}".format(os.path.isfile(self._filepath)))
            reader = self._lib.NewTinyThingReader(
                ctypes.c_char_p(bytes(self._filepath, "UTF-8")),
                ctypes.c_int(tt.fileno()))
            if not self._lib.UnzipToolpath(reader):
                print("Cannot unzip toolpath")
            self._lib.GetToolpath.restype = ctypes.c_char_p
            toolpath = self._lib.GetToolpath(reader)
            print(toolpath)
            if toolpath.decode("UTF-8") != self._toolpath:
                print("Toolpaths do not match")

if __name__ == "__main__":
    test = TinyThingTest()
    test.run()