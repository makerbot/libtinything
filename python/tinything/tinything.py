import ctypes
import json

errors = {
    'ok': 0,
    'not_yet_unzipped': 1,
    'tool_mismatch': 2,
    'bot_type_mismatch': 3,
    'version_mismatch': 4,
    'max_string_length_exceeded': 5
}

class VerificationStruct(ctypes.LittleEndianStructure):
    _fields_ = (
        ("tool", ctypes.c_int),
        ("pid", ctypes.c_uint8)
    )

class MetadataStruct(ctypes.LittleEndianStructure):
    _fields_ = (
        ("extrusion_mass_g", ctypes.c_float),
        ("extrusion_distance_mm", ctypes.c_float),
        ("extruder_temperature", ctypes.c_int),
        ("chamber_temperature", ctypes.c_int),
        ("thing_id", ctypes.c_int),
        ("duration_s", ctypes.c_float),
        ("uses_raft", ctypes.c_bool),
        ("uuid", ctypes.c_char * 100),
        ("material", ctypes.c_char*50),
        ("tool_type", ctypes.c_int),
        ("bot_pid", ctypes.c_uint),
        ("bounding_box_x_min", ctypes.c_float),
        ("bounding_box_x_max", ctypes.c_float),
        ("bounding_box_y_min", ctypes.c_float),
        ("bounding_box_y_max", ctypes.c_float),
        ("bounding_box_z_min", ctypes.c_float),
        ("bounding_box_z_max", ctypes.c_float)
    )

class TinyThing:
    """
    @param lib_path: optional parameter for custom library path
    """
    def __init__(self, zipfile_path:str, fd:int=0, lib_path:str='/usr/lib/libtinything.so'):
        self._libtinything = ctypes.CDLL(lib_path)
        if None is not zipfile_path:
            c_path = ctypes.c_char_p(bytes(zipfile_path.encode('UTF-8')))
        else:
            c_path = ctypes.c_char_p(b"")
        c_fd = ctypes.c_int(fd)
        self.reader = self._libtinything.NewTinyThingReader(c_path, c_fd)

    def _struct_to_dict(self, c_struct):
        """
        Marshalls a structure into a python dict.  Will recurse if a field
        is found to be a nested Structure.
        """
        _dict = {}
        struct_types = (ctypes.Structure, ctypes.LittleEndianStructure, ctypes.BigEndianStructure)
        for (name, _ctype) in c_struct._fields_:
            field = getattr(c_struct, name)
            if isinstance(field, struct_types):
                _dict[name] = self._struct_to_dict(field)
            elif isinstance(field, float):
                # Why are we casting all floats to ints?
                _dict[name] = int(field)
            elif isinstance(field, bytes):
                _dict[name] = field.decode("ascii")
            else:
                _dict[name] = field
        return _dict

    def does_metadata_match(self, tool, pid):
        c_struct = VerificationStruct(tool, pid)
        error = self._libtinything.DoesMetadataMatch(self.reader, ctypes.byref(c_struct))
        if error == errors['not_yet_unzipped']:
            raise NotYetUnzippedException('meta.json')
        else:
            return error

    def unzip_metadata(self):
        return self._libtinything.UnzipMetadata(self.reader)

    def get_metadata(self):
        data = MetadataStruct()
        error = self._libtinything.GetMetadata(self.reader, ctypes.byref(data))
        if error == errors['not_yet_unzipped']:
            raise NotYetUnzippedException('meta.json')
        else:
            return self._struct_to_dict(data)

    def get_slice_profile(self):
        prof_pointer = ctypes.POINTER(ctypes.c_char)()
        error = self._libtinything.GetSliceProfile(self.reader,
                                                   ctypes.byref(prof_pointer))
        prof_dict = json.loads(bytes(ctypes.string_at(prof_pointer))\
                               .decode('UTF-8'))
        return prof_dict

    def __del__(self):
        self._libtinything.DestroyTinyThingReader(self.reader)

class ToolMismatchException(ValueError):
    pass

class MachineMismatchException(ValueError):
    pass

class NotYetUnzippedException(RuntimeError):
    pass

class VersionMismatchException(RuntimeError):
    pass
