#include "tinything_c_interface.hh"

namespace LibTinyThing {

extern "C"
TinyThing* NewTinyThing(const char* filepath){
    TinyThing* tinything = new TinyThing(filepath);
    return tinything;
}

extern "C"
int DestroyTinyThing(TinyThing* tinything){
    delete tinything;
    return 0;
}

extern "C"
bool UnzipToolpath(TinyThing* tinything){
	bool success = tinything->unzipToolpathFile();
	return success;
}

extern "C"
const char* GetToolpath(TinyThing* tinything){
    std::string toolpath = tinything->getToolpathFileContents();
    return toolpath.c_str();
}

}
