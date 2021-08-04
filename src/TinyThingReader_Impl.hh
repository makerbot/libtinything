#include <jsoncpp/json/json.h>
#include "tinything/TinyThingReader.hh"
#include "semver.hh"
namespace LibTinyThing {
class TinyThingReader::Private {
 public:
    Private(const std::string& filePath, int fd);
    ~Private();
    bool hasJsonToolpath() const;
    bool hasGcodeToolpath() const;
    bool hasMetadata() const;
    bool isValid() const;
    bool resetToolpath();
    std::string getToolpathIncr(const int chars);
    int getToolpathIncr(char* buff, int chars);
    bool unzipFile(const std::string& fileName, std::string &output) const;
    static bool parseFile(const std::string& contents, Json::Value* output);

    Error verifyMetadata(const VerificationData& data) const;
    Error getCppOnlyMetadata(Metadata* out) const;

    template <class MetadataType>
    Error getMetadata(MetadataType* out) const;

    Error getSliceProfile(const char** out) const;
    Error getPurgeRoutines(const char** out) const;
    Error getAccelOverrides(const char** out) const;

    const std::string m_filePath;
    const bool m_via_fd;

    std::string m_toolpathFileContents;
    std::string m_isometricSmallThumbnailFileContents;
    std::string m_isometricMediumThumbnailFileContents;
    std::string m_isometricLargeThumbnailFileContents;
    std::string m_smallThumbnailFileContents;
    std::string m_mediumThumbnailFileContents;
    std::string m_largeThumbnailFileContents;
    std::string m_sombreroSmallThumbnailFileContents;
    std::string m_sombreroMediumThumbnailFileContents;
    std::string m_sombreroLargeThumbnailFileContents;
    std::string m_smallSquareThumbnailFileContents;
    std::string m_metadataFileContents;
    std::string m_sliceProfileContents;
    std::string m_purgeRoutineContents;
    std::string m_accelOverridesContents;
    Json::Value m_metadataParsed;

    // variables to support incremental toolpath unzipping
    unzFile m_zipFile;
    bool m_incremental; // are we set to incrementally unzip
    unsigned int m_toolpathSize;
    unsigned int m_toolpathPos;
    SemVer m_metafileVersion;
    static SemVer maxSupportedVersion();
};
}
