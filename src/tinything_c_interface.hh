#include <string>
#include "TinyThingReader.hh"
#include "TinyThingWriter.hh"

namespace LibTinyThing {

// READER FUNCTIONS

extern "C"
TinyThingReader* NewTinyThingReader(const char* filepath);

extern "C"
int DestroyTinyThingReader(TinyThingReader* tinything);

extern "C"
bool UnzipToolpath(TinyThingReader* tinything);

extern "C"
bool UnzipMetadata(TinyThingReader* tinything);

extern "C"
const char* GetToolpath(TinyThingReader* tinything);

extern "C"
const char* GetMetadata(TinyThingReader* tinything);

extern "C"
bool ResetToolpath(TinyThingReader* tinything);

extern "C"
const char* GetToolpathIncr(TinyThingReader* tinything, const int bytes);

// WRITER FUNCTIONS

extern "C"
TinyThingWriter* NewTinyThingWriter(const char* filepath);

extern "C"
int DestroyTinyThingWriter(TinyThingWriter* tinything);

extern "C"
void SetToolpathFile(TinyThingWriter* tinything, const char* filepath);

extern "C"
void setThumbnailDirectory(TinyThingWriter* tinything, const char* filedir);

extern "C"
void SetMetadataFile(TinyThingWriter* tinything, const char* filepath);

extern "C"
bool Zip(TinyThingWriter* tinything);

}
