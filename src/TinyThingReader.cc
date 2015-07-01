#include <bwcoreutils/tool_mappings.hh>

#include "tinything/TinyThingReader.hh"
#include "tinything/TinyThingConstants.hh"
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

#include "TinyThingReader_Impl.hh"
#include "semver.hh"

using namespace LibTinyThing;

TinyThingReader::Private::Private(const std::string& filePath, int fd)
    : m_filePath(filePath),
      m_zipFile(NULL),
      m_incremental(false),
      m_toolpathSize(0),
      m_toolpathPos(0) {
    // if we have a file descriptor, we effectively
    // ignore the path and use the file descriptor instead
    if (fd > 0) {
        m_zipFile = unzOpenFD(filePath.c_str(), fd);
    } else {
        m_zipFile = unzOpen(filePath.c_str());
    }
}

TinyThingReader::Private::~Private() {
    if (m_zipFile != NULL) {
        unzClose(m_zipFile);
    }
}

bool TinyThingReader::Private::hasJsonToolpath() const {
    return (m_zipFile != NULL &&
            unzLocateFile(m_zipFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
            unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::hasMetadata() const {
    return (m_zipFile != NULL &&
            unzLocateFile(m_zipFile, Config::kMetadataFilename.c_str(), 0) == UNZ_OK &&
            unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::isValid() const {
    return (
        m_zipFile != NULL &&
        unzLocateFile(m_zipFile, Config::kMetadataFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kSmallThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kMediumThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kSmallThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::resetToolpath() {
    m_toolpathPos = 0;
    unz_file_info fileInfo;
    bool locateFile = (unzLocateFile(m_zipFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK);
    bool getInfo = (unzGetCurrentFileInfo(m_zipFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK);
    bool openCurrent = (unzOpenCurrentFile(m_zipFile) == UNZ_OK);
    if (locateFile && getInfo && openCurrent) {
      m_toolpathSize = fileInfo.uncompressed_size;
      m_incremental = true;
      return true;
    } else {
      return false;
    }
}

std::string TinyThingReader::Private::getToolpathIncr(const int chars) {
    if (!m_incremental) {
        if(!resetToolpath()) {
            // Oops, we have an error
            return "";
        }
    }
        
    int chars_to_read = chars;
    if (m_toolpathPos >= m_toolpathSize) {
      // at end of file
      return "";
    } else if ((m_toolpathPos + chars) > m_toolpathSize) {
      // reached end of file
      chars_to_read = m_toolpathSize - m_toolpathPos;
    }
    m_toolpathPos += chars_to_read;

    std::string output;
    output.resize(chars_to_read);
    unzReadCurrentFile(m_zipFile,
        const_cast<char*>(output.c_str()),
        chars_to_read);

    return output;
}

bool TinyThingReader::Private::unzipFile(const std::string& fileName, std::string &output) const{
    if (m_zipFile != NULL   &&
        unzLocateFile(m_zipFile, fileName.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(m_zipFile) == UNZ_OK) {
      unz_file_info fileInfo;

      if (unzGetCurrentFileInfo(m_zipFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
        // Create a buffer large enough to hold the uncompressed file
        output.resize(fileInfo.uncompressed_size);
        if (unzReadCurrentFile(m_zipFile,
                               const_cast<char*>(output.c_str()),
                               fileInfo.uncompressed_size)) {
          return true;
        }
      }
    }
    return false;
}

bool TinyThingReader::Private::parseFile(const std::string& contents, Json::Value* output) {
    Json::Reader reader;
    // Just because Json::Reader doesn't follow the style guide about using pointers instead
    // of non-const refs doesn't mean we don't have to
    return reader.parse(contents, *output);
}

TinyThingReader::TinyThingReader::Error
TinyThingReader::Private::verifyMetadata(const VerificationData& data) const {
    if(m_metadataFileContents.empty()) {
        // We have not yet parsed the metadata file, don't even try to use the JSON
        return kNotYetUnzipped;
    }
    if(m_metafileVersion.major == 0) {
        // This slice is old enough to not be versioned, we'll use good old fashioned
        // hope to ensure it's correct
        return Error::kOK;
    } else if(m_metafileVersion.major > 1) {
        return kVersionMismatch;
    } else {
        // Check bot type first, since this is guaranteed to exist even if
        // the print was sliced with a custom profile. Default: a value we
        // will never use as a PID.
        const std::string type
            = m_metadataParsed.get("bot_type", "_9999").asString();
        const size_t pid_idx = type.rfind('_')+1;
        if(std::stoi(type.substr(pid_idx)) != data.pid) {
            return Error::kBotTypeMismatch;
        }
        // Now check the tool, which is allowed to be a JSON null
        // Default: a null
        if(!m_metadataParsed["tool_type"].isNull()) {
            const std::string name
                = m_metadataParsed.get("tool_type",
                                       Json::Value()).asString();
            const bwcoreutils::TYPE meta_tool
                = bwcoreutils::YonkersTool::type_from_type_name(name);
            const bwcoreutils::TYPE current_tool
                = bwcoreutils::YonkersTool(data.tool_id).type();
            if(!bwcoreutils::YonkersTool::toolpaths_equivalent(meta_tool,
                                                               current_tool)) {
                return Error::kToolMismatch;
            }
        }
    }
    return Error::kOK;
}

template<class MetadataType>
TinyThingReader::Error TinyThingReader::Private::getMetadata(MetadataType* out) const {
    switch(m_metafileVersion.major) {
    case -1:{
        return TinyThingReader::Error::kNotYetUnzipped;
    } break;
    case 0:{
        out->extrusion_mass_g
            = m_metadataParsed.get("extrusion_mass_a_grams", 0.f).asFloat()
            + m_metadataParsed.get("extrusion_mass_b_grams", 0.f).asFloat();
        out->extrusion_distance_mm
            = m_metadataParsed.get("extrusion_distance_a_mm", 0.f).asFloat()
            + m_metadataParsed.get("extrusion_distance_b_mm", 0.f).asFloat();
        out->duration_s = m_metadataParsed.get("duration_s", 0.f).asFloat();
        out->extruder_temperature
            = m_metadataParsed.get("toolhead_0_temperature", 0).asInt();
        out->chamber_temperature
            = m_metadataParsed.get("chamber_temperature", 0).asInt();
        out->uses_raft
            = m_metadataParsed["printer_settings"].get("raft", false).asBool();
    } break;
    case 1:{
        out->extrusion_mass_g
            = m_metadataParsed.get("extrusion_mass_g", 0.f).asFloat();
        out->extrusion_distance_mm
            = m_metadataParsed.get("extrusion_distance_mm", 0.f).asFloat();
        out->duration_s
            = m_metadataParsed.get("duration_s", 0.f).asFloat();
        out->extruder_temperature
            = m_metadataParsed.get("extruder_temperature", 0.f).asInt();
        out->chamber_temperature
            = m_metadataParsed.get("chamber_temperature",
                                   Json::Value(0U)).asUInt();
        out->uses_raft
            = m_metadataParsed.get("miracle_config", Json::Value())
            .get("doRaft", false).asBool();
    } break;
    }
    // common output
    out->thing_id = m_metadataParsed.get("thing_id", (int)0).asUInt();
    const std::string uuid = m_metadataParsed.get("uuid", "").asString();
    if (uuid.size() > UUID_MAX_LENGTH) {
        return Error::kMaxStringLengthExceeded;
    }
    uuid.copy(out->uuid, uuid.size());
    //out->uuid[uuid.size()] = '\0';
    return Error::kOK;
}

TinyThingReader::Error TinyThingReader::Private::getCppOnlyMetadata(Metadata* out) const {
    switch(m_metafileVersion.major) {
    case -1:{
        return TinyThingReader::Error::kNotYetUnzipped;
    } break;
    case 0:{
        const Json::Value printer_settings
            = m_metadataParsed.get("printer_settings", Json::Value());
        out->shells = printer_settings.get("shells", (int)0).asInt();
        out->layer_height = printer_settings.get("layer_height", 0.f).asFloat();
        out->infill_density = printer_settings.get("infill", 0.f).asFloat();
        out->uses_support = printer_settings.get("support", false).asBool();
        out->max_flow_rate = 0.f; // Not included in this version
        out->material
            = printer_settings.get("materials", Json::Value())
            .get(Json::ArrayIndex(0), "UNKNOWN").asString();
        out->slicer_name = printer_settings.get("slicer", "UNKNOWN").asString();
    }break;
    case 1:{
        const Json::Value miracle_config
            = m_metadataParsed.get("miracle_config", Json::Value());
        out->shells = miracle_config.get("numberOfShells", (int)0).asInt();
        out->layer_height = miracle_config.get("layerHeight", 0.f).asFloat();
        out->infill_density
            = miracle_config.get("infillDensity", 0.f).asFloat();
        out->uses_support = miracle_config.get("doSupport", false).asBool();
        out->max_flow_rate
            = m_metadataParsed.get("max_flow_rate", 0.f).asFloat();
        out->material = m_metadataParsed.get("material", "UNKNOWN").asString();
        out->slicer_name = miracle_config.get("slicer", "UNKNOWN").asString();
    }break;
    }
    return Error::kOK;
}


TinyThingReader::TinyThingReader(const std::string& filePath, int fd)
    : m_private(new Private(filePath, fd)) {}

TinyThingReader::~TinyThingReader() {
    delete m_private;
}

bool TinyThingReader::unzipMetadataFile() {
  const bool unzipped
    = m_private->unzipFile(Config::kMetadataFilename, 
                             m_private->m_metadataFileContents);
  if(!unzipped) {return unzipped;}
  const bool extracted
    = Private::parseFile(m_private->m_metadataFileContents,
                         &(m_private->m_metadataParsed));
  m_private->m_metafileVersion
    = SemVer(m_private->m_metadataParsed.get("version",
                                             Json::Value("0.0.0"))
             .asString());

  return extracted;
}

bool TinyThingReader::unzipSmallThumbnailFile() {
    return m_private->unzipFile(
        Config::kSmallThumbnailFilename, 
        m_private->m_smallThumbnailFileContents);
}

bool TinyThingReader::unzipMediumThumbnailFile() {
    return m_private->unzipFile(
        Config::kMediumThumbnailFilename, 
        m_private->m_mediumThumbnailFileContents);
}

bool TinyThingReader::unzipLargeThumbnailFile() {
    return m_private->unzipFile(
        Config::kLargeThumbnailFilename, 
        m_private->m_largeThumbnailFileContents);
}

bool TinyThingReader::unzipToolpathFile() {
    m_private->m_incremental = false;
    const bool ok = m_private->unzipFile(
                         Config::kToolpathFilename, 
                         m_private->m_toolpathFileContents);
    if(!ok) {
        return ok;
    } else {
        return m_private->resetToolpath();
    }
}

TinyThingReader::Error TinyThingReader::getMetadata(Metadata* out) const {
    const Error error = m_private->getMetadata<Metadata>(out);
    if(error != kOK) {return error;}
    return m_private->getCppOnlyMetadata(out);
}

TinyThingReader::Error TinyThingReader::getMetadata(CInterfaceMetadata* out) const {
    return m_private->getMetadata<CInterfaceMetadata>(out);
}

void TinyThingReader::getSmallThumbnailFileContents(std::string* contents) const {
    *contents = m_private->m_smallThumbnailFileContents;
}

void TinyThingReader::getMediumThumbnailFileContents(std::string* contents) const{
    *contents = m_private->m_mediumThumbnailFileContents;
}

void TinyThingReader::getLargeThumbnailFileContents(std::string* contents) const{
    *contents = m_private->m_largeThumbnailFileContents;
} 

void TinyThingReader::getToolpathFileContents(std::string* contents) const {
    *contents = m_private->m_toolpathFileContents;
}

bool TinyThingReader::hasJsonToolpath() const{
    return m_private->hasJsonToolpath();
}


bool TinyThingReader::hasMetadata() const {
    return m_private->hasMetadata();
}

bool TinyThingReader::isValid() const{
    return m_private->isValid();
}

// returns true if succesful
bool TinyThingReader::resetToolpath(){
    return m_private->resetToolpath();
}

// if length of return string is < bytes, you have reached end of file
std::string TinyThingReader::getToolpathIncr(const int chars) {
    return m_private->getToolpathIncr(chars);
}

TinyThingReader::TinyThingReader::Error
TinyThingReader::doesMetadataFileMatchConfig(const VerificationData& config) const {
    return m_private->verifyMetadata(config);
}
