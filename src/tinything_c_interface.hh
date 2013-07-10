#include <string>
#include "tinything.hh"

namespace LibTinyThing {

extern "C"
TinyThing* NewTinyThing(const char* filepath);

extern "C"
int DestroyTinyThing(TinyThing* tinything);

extern "C"
bool UnzipToolpath(TinyThing* tinything);

extern "C"
bool UnzipMetadata(TinyThing* tinything);

extern "C"
const char* GetToolpath(TinyThing* tinything);

extern "C"
const char* GetMetadata(TinyThing* tinything);

extern "C"
bool AddMetadata(TinyThing* tinything, char const * const filepath);

extern "C"
bool AddToolpath(TinyThing* tinything, char const * const filepath);

extern "C"
bool AddThumbnail(TinyThing* tinything, char const * const filepath);

extern "C"
bool ResetToolpath(TinyThing* tinything);

extern "C"
const char* GetToolpathIncr(TinyThing* tinything, const int bytes);



}


