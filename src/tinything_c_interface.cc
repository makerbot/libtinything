
#include "tinything/tinything_c_interface.hh"
#include "tinything/TinyThingWriter.hh"
#include "tinything/TinyThingReader.hh"

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

// WRITER FUNCTIONS

extern "C"
TinyThingWriter* NewTinyThingWriter(const char* filepath){
	TinyThingWriter* tinything = new TinyThingWriter(filepath);
    return tinything;
}

extern "C"
int DestroyTinyThingWriter(TinyThingWriter* tinything){
    delete tinything;
    return 0;
}

extern "C"
void SetToolpathFile(TinyThingWriter* tinything, const char* filepath){
	tinything->setToolpathFile(filepath);
}

extern "C"
void SetThumbnailDirectory(TinyThingWriter* tinything, const char* filedir){
	tinything->setThumbnailDirectory(filedir);
}

extern "C"
void SetMetadataFile(TinyThingWriter* tinything, const char* filepath){
	tinything->setMetadataFile(filepath);
}

extern "C"
bool Zip(TinyThingWriter* tinything){
    return tinything->zip();
}

}
