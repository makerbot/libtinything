#include <string>

namespace LibTinyThing {

class TinyThingReader;
class TinyThingWriter;

// READER FUNCTIONS

extern "C"
TinyThingReader* TINYTHING_API NewTinyThingReader(const char* filepath);

extern "C"
int TINYTHING_API DestroyTinyThingReader(TinyThingReader* tinything);

extern "C"
bool TINYTHING_API UnzipToolpath(TinyThingReader* tinything);

extern "C"
bool TINYTHING_API UnzipMetadata(TinyThingReader* tinything);

extern "C"
const char* TINYTHING_API GetToolpath(TinyThingReader* tinything);

extern "C"
const char* TINYTHING_API GetMetadata(TinyThingReader* tinything);

extern "C"
bool TINYTHING_API ResetToolpath(TinyThingReader* tinything);

extern "C"
const char* TINYTHING_API GetToolpathIncr(TinyThingReader* tinything, const int bytes);

// WRITER FUNCTIONS

extern "C"
TinyThingWriter* TINYTHING_API NewTinyThingWriter(const char* filepath);

extern "C"
int TINYTHING_API DestroyTinyThingWriter(TinyThingWriter* tinything);

extern "C"
void TINYTHING_API SetToolpathFile(TinyThingWriter* tinything, const char* filepath);

extern "C"
void TINYTHING_API setThumbnailDirectory(TinyThingWriter* tinything, const char* filedir);

extern "C"
void TINYTHING_API SetMetadataFile(TinyThingWriter* tinything, const char* filepath);

extern "C"
bool TINYTHING_API Zip(TinyThingWriter* tinything);

}
