
#include "tinything_c_interface.hh"
#include "TinyThingWriter.hh"
#include "TinyThingReader.hh"

namespace LibTinyThing {

// READER FUNCTIONS

extern "C"
TinyThingReader* NewTinyThingReader(const char* filepath, int fd){
    TinyThingReader* tinything = new TinyThingReader(filepath, fd);
    return tinything;
}

extern "C"
int DestroyTinyThingReader(TinyThingReader* tinything){
    delete tinything;
    return 0;
}

extern "C"
bool UnzipMetadata(TinyThingReader* tinything){
	bool success = tinything->unzipMetadataFile();
	return success;
}

extern "C"
int DoesMetadataMatch(const TinyThingReader* tinything,
                      const VerificationData* data) {
    return tinything->doesMetadataFileMatchConfig(*data);
}

extern "C"
int GetMetadata(const TinyThingReader* tinything,
                CInterfaceMetadata* out) {
    return tinything->getMetadata(out);
}

extern "C"
int GetSliceProfile(const TinyThingReader* tinything,
                    const char** out) {
    return tinything->getSliceProfile(out);
}

extern "C"
int GetPurgeRoutines(const TinyThingReader* tinything,
                     const char** out) {
    return tinything->getPurgeRoutines(out);
}

extern "C"
int GetAccelOverrides(const TinyThingReader* tinything,
                     const char** out) {
    return tinything->getAccelOverrides(out);
}

extern "C"
bool IsValid(const TinyThingReader* tinything) {
    return tinything->isValid();
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
