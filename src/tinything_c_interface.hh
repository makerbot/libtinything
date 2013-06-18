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
const char* GetToolpath(TinyThing* tinything);

}
