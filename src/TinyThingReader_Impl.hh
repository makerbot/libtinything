#include <jsoncpp/json/json.h>
#include "tinything/TinyThingReader.hh"
#include "semver.hh"
namespace LibTinyThing {
class TinyThingReader::Private {
 public:
    Private(const std::string& filePath, int fd);
    ~Private();
    bool hasJsonToolpath();
    bool isValid();
    bool resetToolpath();
    std::string getToolpathIncr(const int chars);
    bool unzipFile(const std::string& fileName, std::string &output) const;
    static bool parseFile(const std::string& contents, Json::Value* output);

    Error verifyMetadata(const VerificationData& data) const;
    Error getMetadata(Metadata* out) const;
    const std::string m_filePath;
    
    std::string m_toolpathFileContents;
    std::string m_smallThumbnailFileContents;
    std::string m_mediumThumbnailFileContents;
    std::string m_largeThumbnailFileContents;
    std::string m_metadataFileContents;
    Json::Value m_metadataParsed;

    // variables to support incremental toolpath unzipping
    unzFile m_zipFile;
    bool m_incremental; // are we set to incrementally unzip
    unsigned int m_toolpathSize;
    unsigned int m_toolpathPos;
    SemVer m_metafileVersion;
};
}
