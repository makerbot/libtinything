namespace LibTinyThing {

class TinyThingReader;
class TinyThingWriter;
struct VerificationData;
struct Metadata;

// READER FUNCTIONS

extern "C" {
TINYTHING_API TinyThingReader* NewTinyThingReader(const char* filepath, int fd);

TINYTHING_API int DestroyTinyThingReader(TinyThingReader* tinything);

TINYTHING_API bool UnzipMetadata(TinyThingReader* tinything);

TINYTHING_API int DoesMetadataMatch(const TinyThingReader* tinything,
                                    const VerificationData* data);

// WRITER FUNCTIONS

TINYTHING_API TinyThingWriter* NewTinyThingWriter(const char* filepath);

TINYTHING_API int DestroyTinyThingWriter(TinyThingWriter* tinything);

TINYTHING_API void SetToolpathFile(TinyThingWriter* tinything, const char* filepath);

TINYTHING_API void setThumbnailDirectory(TinyThingWriter* tinything, const char* filedir);

TINYTHING_API void SetMetadataFile(TinyThingWriter* tinything, const char* filepath);

TINYTHING_API bool Zip(TinyThingWriter* tinything);
}

}
