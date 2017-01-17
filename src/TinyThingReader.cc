#include <bwcoreutils/tool_mappings.hh>

#include "tinything/TinyThingReader.hh"
#include "tinything/TinyThingConstants.hh"
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

#include "TinyThingReader_Impl.hh"
#include "semver.hh"

#include <sys/stat.h>

#include <iostream>
#include <string.h>

using namespace LibTinyThing;

#define ENSURE_ARRAY(obj, array_key, index, def)\
  obj.get(array_key, Json::arrayValue).get(index, def)
  

Metadata::Metadata() : extrusion_mass_g{0.f, 0.f},
                       extrusion_distance_mm{0,0},
                       extruder_temperature{0,0},
                       chamber_temperature(0),
                       shells(0),
                       layer_height(0),
                       infill_density(0),
                       uses_support(false),
                       duration_s(0),
                       max_flow_rate{0, 0},
                       thing_id(0),
                       uses_raft(false),
                       uuid(),
                       material{"UNKNOWN","UNKNOWN"},
                       slicer_name("UNKNOWN"),
                       tool_type{bwcoreutils::TYPE::UNKNOWN_TYPE, bwcoreutils::TYPE::UNKNOWN_TYPE},
                       bot_pid(9999) {}

TinyThingReader::Private::Private(const std::string& filePath, int fd)
    : m_filePath(filePath),
      m_via_fd(fd > 0),
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
    // Make a string to hold the data
    std::string buff;
    // Resize it to hold enough chars
    buff.resize(chars);
    // Get a mutable pointer to the chars. This accomplishes the same thing
    // as const_casting c_str(), but cleaner.
    char* buff_ptr = &buff.front();
    // Read into the string
    const int num_read = getToolpathIncr(buff_ptr, chars);
    // chop off the end if smaller
    buff.resize(num_read);
    return buff;
}

int TinyThingReader::Private::getToolpathIncr(char* buff, int chars) {
    if (!m_incremental) {
        if(!resetToolpath()) {
            // Oops, we have an error
            return 0;
        }
    }
        
    int chars_to_read = chars;
    if (m_toolpathPos >= m_toolpathSize) {
      // at end of file
      return 0;
    } else if ((m_toolpathPos + chars) > m_toolpathSize) {
      // reached end of file
      chars_to_read = m_toolpathSize - m_toolpathPos;
    }
    m_toolpathPos += chars_to_read;

    unzReadCurrentFile(m_zipFile,
        buff,
        chars_to_read);

    return chars_to_read;
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

TinyThingReader::Error
TinyThingReader::Private::verifyMetadata(const VerificationData& data) const {
    if(m_metadataFileContents.empty()) {
        // We have not yet parsed the metadata file, don't even try to use the JSON
        return kNotYetUnzipped;
    }
    if(m_metafileVersion.major == 0) {
        // This slice is old enough to not be versioned, we'll use good old fashioned
        // hope to ensure it's correct
        return Error::kOK;
        // Metafile version 2 indicates that the jsontoolpath may contain
        // comments intended to help toolpathviz, per SLIC-356
    } else if(m_metafileVersion.major <= 2) {
        // Check bot type first, since this is guaranteed to exist even if
        // the print was sliced with a custom profile. Default: a value we
        // will never use as a PID.
        const std::string type
            = m_metadataParsed.get("bot_type", "_9999").asString();
        const size_t pid_idx = type.rfind('_')+1;
        if(std::stoi(type.substr(pid_idx), nullptr, 16) != data.pid) {
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
                = bwcoreutils::YonkersTool(data.tool_id[0]).type();
            if(!bwcoreutils::YonkersTool::toolpaths_equivalent(meta_tool,
                                                               current_tool)) {
                return Error::kToolMismatch;
            }
        }
    } else if (m_metafileVersion.major == 3) {
        const std::string type
            = m_metadataParsed.get("bot_type", "_9999").asString();
        const size_t pid_idx = type.rfind('_')+1;
        if(std::stoi(type.substr(pid_idx), nullptr, 16) != data.pid) {
            return Error::kBotTypeMismatch;
        }
        // Now check the tool, which must be a list of the right length, but may
        // contain NULL
        if(m_metadataParsed["tool_type"].size() != data.tool_count) {
            return Error::kToolMismatch;
        } else {
            for (size_t i=0; i<data.tool_count; i++) {
                if (m_metadataParsed["tool_type"][Json::ArrayIndex(i)].isNull()) {
                    continue;
                }
                const std::string name
                    = m_metadataParsed["tool_type"][Json::ArrayIndex(i)]
                  .asString();
                const bwcoreutils::TYPE meta_tool
                    = bwcoreutils::YonkersTool::type_from_type_name(name);
                const bwcoreutils::TYPE current_tool
                    = bwcoreutils::YonkersTool(data.tool_id[i]).type();
                if(!bwcoreutils::YonkersTool::toolpaths_equivalent(meta_tool,
                                                               current_tool)) {
                    return Error::kToolMismatch;
            }
          }
        }
    } else {
        return Error::kVersionMismatch;
    }
    return Error::kOK;
}

template<class MetadataType>
TinyThingReader::Error TinyThingReader::Private::getMetadata(MetadataType* out) const {
    if (m_metafileVersion.major == -1) {
        return TinyThingReader::Error::kNotYetUnzipped;
    } else if (m_metafileVersion == SemVer(0, 0, 0)) {
        out->extrusion_mass_g[0]
            = m_metadataParsed.get("extrusion_mass_a_grams", 0.f).asFloat()
            + m_metadataParsed.get("extrusion_mass_b_grams", 0.f).asFloat();
        out->extrusion_mass_g[1] = 0;
        out->extrusion_distance_mm[0]
            = m_metadataParsed.get("extrusion_distance_a_mm", 0.f).asFloat()
            + m_metadataParsed.get("extrusion_distance_b_mm", 0.f).asFloat();
        out->extrusion_distance_mm[1] = 0;
        out->duration_s = m_metadataParsed.get("duration_s", 0.f).asFloat();
        out->extruder_temperature[0]
            = m_metadataParsed.get("toolhead_0_temperature", 0).asInt();
        out->extruder_temperature[1] = 0;
        out->chamber_temperature
            = m_metadataParsed.get("chamber_temperature", 0).asInt();
        out->uses_raft
            = m_metadataParsed["printer_settings"].get("raft", false).asBool();
        out->tool_type[0] = bwcoreutils::TYPE::UNKNOWN_TYPE;
        out->tool_type[1] = bwcoreutils::TYPE::UNKNOWN_TYPE;
        out->bot_pid = 9999;
        const std::string material = m_metadataParsed["printer_settings"]
            .get("materials", Json::Value(Json::ValueType::arrayValue))
            .get(Json::ArrayIndex(0), "UNKNOWN").asString();
        if (material.size() > MATERIAL_MAX_LENGTH) {
            return Error::kMaxStringLengthExceeded;
        }
        memset(out->material[0], 0, sizeof(char)*MATERIAL_MAX_LENGTH);
        material.copy(out->material[0], material.size());
        out->material[1][0] = 0;
        out->extruder_count = 1;
    } else {
      // All metafiles from version 1.0.0 on have this data
        if (m_metafileVersion.major <= 2) {
            out->extrusion_mass_g[0]
                = m_metadataParsed.get("extrusion_mass_g", 0.f).asFloat();
            out->extrusion_mass_g[1] = 0;
            out->extrusion_distance_mm[0]
                = m_metadataParsed.get("extrusion_distance_mm", 0.f).asFloat();
            out->extrusion_distance_mm[1] = 0;
            out->extruder_temperature[0]
                = m_metadataParsed.get("extruder_temperature", 0.f).asInt();
            out->extruder_temperature[1] = 0;
            out->extruder_count = 1;
            const std::string material
              = m_metadataParsed.get("material", "UNKNOWN").asString();
            if (material.size() > MATERIAL_MAX_LENGTH) {
              return Error::kMaxStringLengthExceeded;
            }
            memset(out->material[0], 0, sizeof(char)*MATERIAL_MAX_LENGTH);
            material.copy(out->material[0], material.size());
            out->material[1][0] = 0;
            if (!m_metadataParsed["tool_type"].isNull()) {
              out->tool_type[0]
                = bwcoreutils::YonkersTool
                ::type_from_type_name(m_metadataParsed.get("tool_type",
                                                           "unknown")
                                      .asString());
            } else {
              out->tool_type[0] = bwcoreutils::TYPE::UNKNOWN_TYPE;
            }
            out->tool_type[1] = bwcoreutils::TYPE::UNKNOWN_TYPE;
        } else if (m_metafileVersion.major == 3) {
            out->extruder_count
                = std::max(2, (int)m_metadataParsed["extruder_temperature"].size());
            for (size_t i=0;
                 i<std::max(2, out->extruder_count);
                 i++) {
                out->extrusion_mass_g[i]
                    = ENSURE_ARRAY(m_metadataParsed, "extrusion_mass_g",
                                   i, 0.f).asFloat();
                out->extrusion_distance_mm[i]
                    = ENSURE_ARRAY(m_metadataParsed, "extrusion_distance_mm",
                                   i, 0.f).asFloat();
                out->extruder_temperature[i]
                    = ENSURE_ARRAY(m_metadataParsed, "extruder_temperature",
                                   i, 0).asInt();
                const std::string material
                    = ENSURE_ARRAY(m_metadataParsed, "material",
                                   i, "").asString();
                if (material.size() > MATERIAL_MAX_LENGTH) {
                    return Error::kMaxStringLengthExceeded;
                }
                memset(out->material[i], 0, sizeof(char)*MATERIAL_MAX_LENGTH);
                material.copy(out->material[i], material.size());
                out->tool_type[i]
                    = bwcoreutils::YonkersTool
                    ::type_from_type_name(ENSURE_ARRAY(m_metadataParsed,
                                                       "tool_type",
                                                       i, "unknown")
                                          .asString());
            }
        }
        out->duration_s
            = m_metadataParsed.get("duration_s", 0.f).asFloat();
        out->chamber_temperature
            = m_metadataParsed.get("chamber_temperature",
                                   Json::Value(0U)).asUInt();
        out->uses_raft
            = m_metadataParsed.get("miracle_config", Json::Value())
            .get("doRaft", false).asBool();
        const std::string type
            = m_metadataParsed.get("bot_type", "_9999").asString();
        const size_t pid_idx = type.rfind('_')+1;
        out->bot_pid = std::stoi(type.substr(pid_idx), nullptr, 16);

        if (m_metafileVersion > SemVer(1, 0, 0)) {
            out->bounding_box_x_min
                = m_metadataParsed.get("bounding_box_x_min",
                                       Json::Value(0.f)).asFloat();
            out->bounding_box_x_max
                = m_metadataParsed.get("bounding_box_x_max",
                                       Json::Value(0.f)).asFloat();
            out->bounding_box_y_min
                = m_metadataParsed.get("bounding_box_y_min",
                                       Json::Value(0.f)).asFloat();
            out->bounding_box_y_max
                = m_metadataParsed.get("bounding_box_y_max",
                                       Json::Value(0.f)).asFloat();
            out->bounding_box_z_min
                = m_metadataParsed.get("bounding_box_z_min",
                                       Json::Value(0.f)).asFloat();
            out->bounding_box_z_max
                = m_metadataParsed.get("bounding_box_z_max",
                                       Json::Value(0.f)).asFloat();
        }
    }

    // common output
    out->thing_id = m_metadataParsed.get("thing_id", (int)0).asUInt();
    if (!m_via_fd) {
        struct stat stat_buffer;
        // I'm going to ignore the return value here because if we've gotten to
        // this point we damn well have a viable file
        stat(m_filePath.c_str(), &stat_buffer);
        out->file_size = stat_buffer.st_size;
    } else {
        // File size is meaningless for drm prints because we stream them
        out->file_size = 0;
    }
    const std::string uuid = m_metadataParsed.get("uuid", "").asString();
    if (uuid.size() > UUID_MAX_LENGTH) {
        return Error::kMaxStringLengthExceeded;
    }
    uuid.copy(out->uuid, uuid.size());

    return Error::kOK;
}

TinyThingReader::Error TinyThingReader::Private::getCppOnlyMetadata(Metadata* out) const {
    if (m_metafileVersion.major == -1) {
        return TinyThingReader::Error::kNotYetUnzipped;
    } else if (m_metafileVersion.major == 0) {
        const Json::Value printer_settings
            = m_metadataParsed.get("printer_settings", Json::Value());
        out->shells = printer_settings.get("shells", (int)0).asInt();
        out->layer_height = printer_settings.get("layer_height", 0.f).asFloat();
        out->infill_density = printer_settings.get("infill", 0.f).asFloat();
        out->uses_support = printer_settings.get("support", false).asBool();
        // Not included in this version
        out->max_flow_rate[0] = out->max_flow_rate[1] = 0.f;
        out->slicer_name = printer_settings.get("slicer", "UNKNOWN").asString();
    } else {
        const Json::Value miracle_config
            = m_metadataParsed.get("miracle_config", Json::Value());
        out->shells = miracle_config.get("numberOfShells", (int)0).asInt();
        out->layer_height = miracle_config.get("layerHeight", 0.f).asFloat();
        out->infill_density
            = miracle_config.get("infillDensity", 0.f).asFloat();
        out->uses_support = miracle_config.get("doSupport", false).asBool();
        out->slicer_name = miracle_config.get("slicer", "UNKNOWN").asString();
        if (m_metafileVersion.major < 3) {
            out->max_flow_rate[0]
                = m_metadataParsed.get("max_flow_rate", 0.f).asFloat();
            out->max_flow_rate[1] = 0;          
        } else {
            out->max_flow_rate[0] = 0;
            out->max_flow_rate[1] = 0;
        }
    }
    return Error::kOK;
}

TinyThingReader::Error
TinyThingReader::Private::getSliceProfile(const char** out) const {
    if (m_sliceProfileContents.empty()) {
        *out = nullptr;
        return TinyThingReader::kNotYetUnzipped;
    }
    *out = m_sliceProfileContents.c_str();
    return TinyThingReader::kOK;
}


TinyThingReader::TinyThingReader(const std::string& filePath, int fd)
    : m_private(new Private(filePath, fd)) {}

TinyThingReader::~TinyThingReader() {
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
  if (extracted) {
      auto fw = Json::FastWriter();
      m_private->m_sliceProfileContents
          = fw.write(m_private->m_metadataParsed.get("miracle_config",
                                                     Json::Value(Json::ValueType::objectValue)));
  }
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

TinyThingReader::Error TinyThingReader::getSliceProfile(const char** out) const {
    return m_private->getSliceProfile(out);
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

std::string TinyThingReader::getToolpathFileContents() const {
    return m_private->m_toolpathFileContents;
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

std::string TinyThingReader::getMetadataFileContents() const {
    return m_private->m_metadataFileContents;
}

// if length of return string is < bytes, you have reached end of file
std::string TinyThingReader::getToolpathIncr(const int chars) {
    return m_private->getToolpathIncr(chars);
}

int TinyThingReader::getToolpathIncr(char* buff, const int chars) {
    return m_private->getToolpathIncr(buff, chars);
}

TinyThingReader::Error
TinyThingReader::doesMetadataFileMatchConfig(const VerificationData& config) const {
    return m_private->verifyMetadata(config);
}
