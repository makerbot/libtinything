import ctypes

errors = {
    'ok': 0,
    'not_yet_unzipped': 1,
    'tool_mismatch': 2,
    'bot_type_mismatch': 3,
    'version_mismatch': 4
}

class VerificationStruct(ctypes.LittleEndianStructure):
    _fields_ = (
        ("tool", ctypes.c_int),
        ("pid", ctypes.c_uint8)
    )

class TinyThing:
    path = '/usr/lib/libtinything.so'
    def __init__(self, zipfile_path:str, fd:int=0):
        self._libtinything = ctypes.CDLL(self.path)
        if None is not zipfile_path:
            c_path = ctypes.c_char_p(zipfile_path)
        else:
            c_path = ctypes.c_char_p("")
        c_fd = ctypes.c_int(fd)
        self.reader = self._libtinything.NewTinyThingReader(c_path, c_fd)

    def does_metadata_match(self, tool, pid):
        c_struct = VerificationStruct(tool, pid)
        error = self._libtinything.DoesMetadataMatch(self.reader, ctypes.byref(c_struct))
        if error == errors['not_yet_unzipped']:
            raise NotYetUnzippedException('meta.json')
        else:
            return error

    def unzip_metadata(self):
        return self._libtinything.UnzipMetadata(self.reader)


class ToolMismatchException(ValueError):
    pass

class MachineMismatchException(ValueError):
    pass

class NotYetUnzippedException(RuntimeError):
    pass

class VersionMismatchException(RuntimeError):
    pass
