#include "tinything_c_interface.hh"

namespace LibTinyThing {

// READER FUNCTIONS

extern "C"
TinyThingReader* NewTinyThingReader(const char* filepath){
    TinyThingReader* tinything = new TinyThingReader(filepath);
    return tinything;
}

extern "C"
int DestroyTinyThingReader(TinyThingReader* tinything){
    delete tinything;
    return 0;
}

extern "C"
bool UnzipToolpath(TinyThingReader* tinything){
	bool success = tinything->unzipToolpathFile();
	return success;
}

extern "C"
bool UnzipMetadata(TinyThingReader* tinything){
	bool success = tinything->unzipMetadataFile();
	return success;
}

extern "C"
const char* GetToolpath(TinyThingReader* tinything){
    std::string toolpath = tinything->getToolpathFileContents();
    return toolpath.c_str();
}

extern "C"
const char* GetMetadata(TinyThingReader* tinything){
    std::string metdata = tinything->getMetadataFileContents();
    return metdata.c_str();
}

extern "C"
bool ResetToolpath(TinyThingReader* tinything){
    return tinything->resetToolpath();
}

extern "C"
const char* GetToolpathIncr(TinyThingReader* tinything, const int bytes){
    return tinything->getToolpathIncr(bytes).c_str();
}

}
