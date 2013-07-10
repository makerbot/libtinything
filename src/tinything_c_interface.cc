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
bool UnzipMetadata(TinyThing* tinything){
	bool success = tinything->unzipMetadataFile();
	return success;
}

extern "C"
const char* GetToolpath(TinyThing* tinything){
    std::string toolpath = tinything->getToolpathFileContents();
    return toolpath.c_str();
}

extern "C"
const char* GetMetadata(TinyThing* tinything){
    std::string metdata = tinything->getMetadataFileContents();
    return metdata.c_str();
}

extern "C"
bool AddMetadata(TinyThing* tinything, char const * const filepath){
	std::string const str_filepath(filepath);
	return tinything->addMetadataFile(str_filepath);
}

extern "C"
bool AddToolpath(TinyThing* tinything, char const * const filepath){
	std::string const str_filepath(filepath);
	return tinything->addToolpathFile(str_filepath);
}

extern "C"
bool AddThumbnail(TinyThing* tinything, char const * const filepath){
	std::string const str_filepath(filepath);
	return tinything->addThumbnailFile(str_filepath);
}

extern "C"
bool ResetToolpath(TinyThing* tinything){
    return tinything->resetToolpath();
}

extern "C"
const char* GetToolpathIncr(TinyThing* tinything, const int bytes){
    return tinything->getToolpathIncr(bytes).c_str();
}

}
