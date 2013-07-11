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



}


