namespace LibTinyThing {

class TinyThingReader;
class TinyThingWriter;
struct VerificationData;
struct CInterfaceMetadata;

// READER FUNCTIONS

extern "C" {
TinyThingReader* NewTinyThingReader(const char* filepath, int fd);

int DestroyTinyThingReader(TinyThingReader* tinything);

bool UnzipMetadata(TinyThingReader* tinything);

int DoesMetadataMatch(const TinyThingReader* tinything,
                                    const VerificationData* data);

int GetMetadata(const TinyThingReader* tinything,
                              CInterfaceMetadata* out);

int GetSliceProfile(const TinyThingReader* tinything,
                                  const char** out);

int GetPurgeRoutines(const TinyThingReader* tinything,
                                   const char** out);

int GetAccelOverrides(const TinyThingReader* tinything,
                                   const char** out);

bool IsValid(const TinyThingReader* tinything);

// WRITER FUNCTIONS

TinyThingWriter* NewTinyThingWriter(const char* filepath);

int DestroyTinyThingWriter(TinyThingWriter* tinything);

void SetToolpathFile(TinyThingWriter* tinything, const char* filepath);

void SetThumbnailDirectory(TinyThingWriter* tinything, const char* filedir);

void SetMetadataFile(TinyThingWriter* tinything, const char* filepath);

bool Zip(TinyThingWriter* tinything);
}

}
