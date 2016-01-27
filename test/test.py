import os
import sys
import unittest

# This tests the python interface for reading tinything files
# TODO(pshaw): integrate with scons!

# Be sure to run this with Python3!

# Also be sure to have compiled libtinything for host, otherwise
# you will get an obscure message about how the library does not
# exist.

current_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(current_dir, "..", "python"))
import tinything.tinything

class TestTinyThing(unittest.TestCase):
    def _create_tinything(self, filename):
        libname = "libtinything.so"
        libpath = os.path.join(current_dir, "..", "obj", libname)
        # load tinything
        filepath = os.path.join(current_dir, filename)
        return tinything.tinything.TinyThing(filepath, lib_path=libpath)

    def test_metadata(self):
        tt = self._create_tinything("test.makerbot")
        tt.unzip_metadata()
        metadata = tt.get_metadata()
        self.assertEqual(metadata["uuid"], "56386a26-bacb-49d6-bd77-5f4ce7d72563")
        self.assertEqual(metadata["extruder_temperature"], 215)

if __name__ == "__main__":
    unittest.main()
