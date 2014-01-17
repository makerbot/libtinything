#include <string>

namespace LibTinyThing {

class TinyThingReader;
class TinyThingWriter;

// READER FUNCTIONS

extern "C" {
TINYTHING_API TinyThingReader* NewTinyThingReader(const char* filepath);

TINYTHING_API int DestroyTinyThingReader(TinyThingReader* tinything);

TINYTHING_API bool UnzipToolpath(TinyThingReader* tinything);

TINYTHING_API bool UnzipMetadata(TinyThingReader* tinything);

TINYTHING_API const char* GetToolpath(TinyThingReader* tinything);

TINYTHING_API const char* GetMetadata(TinyThingReader* tinything);

TINYTHING_API bool ResetToolpath(TinyThingReader* tinything);

TINYTHING_API const char* GetToolpathIncr(TinyThingReader* tinything, const int bytes);

// WRITER FUNCTIONS

TINYTHING_API TinyThingWriter* NewTinyThingWriter(const char* filepath);

TINYTHING_API int DestroyTinyThingWriter(TinyThingWriter* tinything);

TINYTHING_API void SetToolpathFile(TinyThingWriter* tinything, const char* filepath);

TINYTHING_API void SetThumbnailDirectory(TinyThingWriter* tinything, const char* filedir);

TINYTHING_API void SetMetadataFile(TinyThingWriter* tinything, const char* filepath);

TINYTHING_API bool Zip(TinyThingWriter* tinything);
}

}
