// Copyright MakerBot Inc. 2017

#include <bwcoreutils/tool_mappings.hh>
#include <mbcoreutils/jsoncpp_wrappers.h>

#include <sys/stat.h>
#include <string.h>
#include <algorithm>
#include <cstdlib>
#include <cmath>

#include "tinything/TinyThingReader.hh"
#include "tinything/TinyThingConstants.hh"
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

#include "TinyThingReader_Impl.hh"
#include "semver.hh"

using namespace LibTinyThing;  // NOLINT(build/namespaces)

SemVer TinyThingReader::Private::maxSupportedVersion() {
    return SemVer(3, 0, 0);
}

Metadata::Metadata() : extrusion_mass_g(),
                       extrusion_distance_mm(),
                       extruder_temperature(),
                       chamber_temperature(0),
                       buildplane_target_temperature(0),
                       platform_temperature(0),
                       shells(0),
                       layer_height(0),
                       infill_density(0),
                       uses_support(false),
                       duration_s(0),
                       max_flow_rate(),
                       thing_id(0),
                       uses_raft(false),
                       uuid(),
                       material(),
                       override_tau_accel(0),
                       override_tau_decel(0),
                       override_winding_current(0),
                       slicer_name("UNKNOWN"),
                       tool_type(),
                       bot_pid(9999) {
    // Because we still support VS 2013 which is not C++11 compliant,
    // we have to initialize these in a pretty crappy way.
    for (int i = 0; i < 2; ++i) {
        extrusion_mass_g[i] = 0.f;
        extrusion_distance_mm[i] = 0;
        extruder_temperature[i] = 0;
        max_flow_rate[i] = 0;
        strcpy(material[i], "UNKNOWN");
        tool_type[i] = bwcoreutils::TYPE::UNKNOWN_TYPE;
    }
}

TinyThingReader::Private::Private(const std::string& filePath, int fd)
    : m_filePath(filePath),
      m_via_fd(fd > 0),
      m_metadataParsed(Json::objectValue),
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
            unzLocateFile(m_zipFile,
                          Config::kJsonToolpathFilename.c_str(),
                          0) == UNZ_OK &&
            unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::hasGcodeToolpath() const {
    return (m_zipFile != NULL &&
            unzLocateFile(m_zipFile,
                          Config::kGcodeToolpathFilename.c_str(),
                          0) == UNZ_OK &&
            unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::hasMetadata() const {
    return (m_zipFile != NULL &&
            unzLocateFile(m_zipFile,
                          Config::kMetadataFilename.c_str(),
                          0) == UNZ_OK &&
            unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::isValid() const {
    return (
        m_zipFile != NULL &&
        unzLocateFile(m_zipFile,
                      Config::kMetadataFilename.c_str(),
                      0) == UNZ_OK &&
        unzLocateFile(m_zipFile,
                      Config::kSmallThumbnailFilename.c_str(),
                      0) == UNZ_OK &&
        unzLocateFile(m_zipFile,
                      Config::kMediumThumbnailFilename.c_str(),
                      0) == UNZ_OK &&
        unzLocateFile(m_zipFile,
                      Config::kLargeThumbnailFilename.c_str(),
                      0) == UNZ_OK &&
        (unzLocateFile(m_zipFile,
                      Config::kJsonToolpathFilename.c_str(),
                      0) == UNZ_OK ||
            unzLocateFile(m_zipFile,
                          Config::kGcodeToolpathFilename.c_str(),
                          0) == UNZ_OK) &&
        unzOpenCurrentFile(m_zipFile) == UNZ_OK);
}

bool TinyThingReader::Private::resetToolpath() {
    m_toolpathPos = 0;
    unz_file_info fileInfo;
    bool locateFile = (unzLocateFile(m_zipFile,
                                     Config::kJsonToolpathFilename.c_str(),
                                     0) == UNZ_OK) ||
                      (unzLocateFile(m_zipFile,
                                     Config::kGcodeToolpathFilename.c_str(),
                                     0) == UNZ_OK);
    bool getInfo = (unzGetCurrentFileInfo(m_zipFile, &fileInfo,
                                          NULL, 0, NULL, 0, NULL, 0) == UNZ_OK);
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
        if (!resetToolpath()) {
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

bool TinyThingReader::Private::unzipFile(const std::string& fileName,
                                         std::string &output) const {
    if (m_zipFile != NULL   &&
        unzLocateFile(m_zipFile, fileName.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(m_zipFile) == UNZ_OK) {
      unz_file_info fileInfo;

      if (unzGetCurrentFileInfo(m_zipFile, &fileInfo,
                                NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
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

bool TinyThingReader::Private::parseFile(const std::string& contents,
                                         Json::Value* output) {
    Json::Reader reader;
    // Just because Json::Reader doesn't follow the style guide about using
    // pointers instead of non-const refs doesn't mean we don't have to
    return reader.parse(contents, *output);
}

// Compute the numeric PID of the printer from a bot type string.  If a valid
// PID PID (a positive integer less than 2^16) cannot be determined, we return
// 9999.
static unsigned int parse_pid(const std::string & type) {
    size_t pid_idx = type.rfind('_');
    if (pid_idx != std::string::npos) {
        long bot_pid = std::strtol(type.substr(pid_idx+1).c_str(), nullptr, 16);
        if (bot_pid <= 0 || bot_pid > 0xffff) {
            bot_pid = 9999;
        }
        return bot_pid;
    } else if (type == "sketch") {  // :(
        return 0xE5;
    } else {
        return 9999;
    }
}

TinyThingReader::Error
TinyThingReader::Private::verifyMetadata(const VerificationData& data) const {
    using namespace MakerBot::SafeJson;  // NOLINT(build/namespaces)
    if (m_metadataFileContents.empty()) {
        // We have not yet parsed the metadata file, don't even try to use it
        return kNotYetUnzipped;
    }
    if (m_metafileVersion > maxSupportedVersion()) {
        return Error::kVersionMismatch;
    }
    if (m_metafileVersion.major == 0) {
        // This slice is old enough to not be versioned, we'll use good old
        // hope to ensure it's correct
        return Error::kOK;
        // Metafile version 2 indicates that the jsontoolpath may contain
        // comments intended to help toolpathviz, per SLIC-356
    } else if (m_metafileVersion.major <= 2) {
        // Check bot type first, since this is guaranteed to exist even if
        // the print was sliced with a custom profile. Default: a value we
        // will never use as a PID.
        const std::string type = get_leaf(m_metadataParsed, "bot_type", "");
        if (parse_pid(type) != data.pid) {
            return Error::kBotTypeMismatch;
        }
        // Now check the tool, which is allowed to be a JSON null
        if (m_metadataParsed.isMember("tool_type")
            && m_metadataParsed["tool_type"].isNull()) {
            // Ignore nulls when checking tool types
        } else {
            const std::string name = get_leaf(m_metadataParsed, "tool_type",
                                              "unknown");
            const bwcoreutils::TYPE meta_tool
                = bwcoreutils::YonkersTool::type_from_type_name(name);
            const bwcoreutils::TYPE current_tool
                = bwcoreutils::YonkersTool(data.tool_id[0]).type();
            if (!bwcoreutils::YonkersTool::toolpaths_equivalent(meta_tool,
                                                                current_tool)) {
                return Error::kToolMismatch;
            }
        }
    } else if (m_metafileVersion.major == 3) {
        const std::string type = get_leaf(m_metadataParsed, "bot_type", "");
        if (parse_pid(type) != data.pid) {
            return Error::kBotTypeMismatch;
        }
        // Now check the tool, which must be a list of the right length, but may
        // contain NULL
        auto tools = get_arr(m_metadataParsed, "tool_types");
        auto tool_extrusion_distances = get_arr(m_metadataParsed,
                                            "extrusion_distances_mm");
        if (tools.size() != data.tool_count) {
            return Error::kToolMismatch;
        } else {
            for (size_t i = 0; i < data.tool_count; i++) {
                Json::ArrayIndex ai(i);
                const float dist = get_leaf(tool_extrusion_distances, ai, 0);
                if (tools[ai].isNull() || dist <= 0 ||
                    tool_extrusion_distances[ai].isNull()) {
                    continue;
                }
                const std::string name = get_leaf(tools, ai, "unknown");
                const bwcoreutils::TYPE meta_tool
                    = bwcoreutils::YonkersTool::type_from_type_name(name);
                const bwcoreutils::TYPE current_tool
                    = bwcoreutils::YonkersTool(data.tool_id[i]).type();
                if (!bwcoreutils::YonkersTool::toolpaths_equivalent(meta_tool,
                                                               current_tool)) {
                    return Error::kToolMismatch;
                }
            }
        }
    }
    // If you are adding support for a new version, please make sure to update
    // maxSupportedVersion
    return Error::kOK;
}

namespace {  // Making static free (non-member) functions:
    int chamberTempFromBuildplane(const int buildplane_temp) {
        return (buildplane_temp > 40)?
            std::round((buildplane_temp + 13) / 1.333):
            (buildplane_temp);
    }
    int buildplaneTempFromChamber(const int chamber_temp) {
        return (chamber_temp > 40)?
            std::round((chamber_temp * 1.333) - 13):
            (chamber_temp);
    }
}

template<class MetadataType>
TinyThingReader::Error
TinyThingReader::Private::getMetadata(MetadataType* out) const {
    using namespace MakerBot::SafeJson;  // NOLINT(build/namespaces)
    if (m_metadataFileContents.empty()) {
        // We have not yet parsed the metadata file, don't even try to use the
        // JSON
        return TinyThingReader::Error::kNotYetUnzipped;
    }
    if (m_metafileVersion > maxSupportedVersion()) {
        return TinyThingReader::Error::kVersionMismatch;
    }  else if (m_metafileVersion == SemVer(0, 0, 0)) {
        out->extrusion_mass_g[0]
            = get_leaf(m_metadataParsed, "extrusion_mass_a_grams", 0.f)
            + get_leaf(m_metadataParsed, "extrusion_mass_b_grams", 0.f);
        out->extrusion_mass_g[1] = 0;
        out->extrusion_distance_mm[0]
            = get_leaf(m_metadataParsed, "extrusion_distance_a_mm", 0.f)
            + get_leaf(m_metadataParsed, "extrusion_distance_b_mm", 0.f);
        out->extrusion_distance_mm[1] = 0;
        out->duration_s = get_leaf(m_metadataParsed, "duration_s", 0.f);
        out->extruder_temperature[0]
            = get_leaf(m_metadataParsed, "toolhead_0_temperature",
                       static_cast<int>(0));
        out->extruder_temperature[1] = 0;
        out->chamber_temperature
            = get_leaf(m_metadataParsed, "chamber_temperature", 0);
        auto printer_settings = get_obj(m_metadataParsed, "printer_settings");
        out->uses_raft = get_leaf(printer_settings, "raft", false);
        out->tool_type[0] = bwcoreutils::TYPE::UNKNOWN_TYPE;
        out->tool_type[1] = bwcoreutils::TYPE::UNKNOWN_TYPE;
        out->bot_pid = 9999;
        const std::string material
          = get_leaf(get_arr(printer_settings, "materials"),
                     Json::ArrayIndex(0), "UNKNOWN");
        if (material.size() >= MATERIAL_MAX_LENGTH) {
            return Error::kMaxStringLengthExceeded;
        }
        memset(out->material[0], 0, sizeof(char)*MATERIAL_MAX_LENGTH);
        material.copy(out->material[0], material.size());
        out->material[1][0] = 0;
        out->extruder_count = 1;
    } else {
      // All metafiles from version 1.0.0 on have this data
        if (m_metafileVersion.major <= 2) {
            out->extrusion_mass_g[0] = get_leaf(m_metadataParsed,
                                                "extrusion_mass_g", 0.f);
            out->extrusion_mass_g[1] = 0;
            out->extrusion_distance_mm[0] = get_leaf(m_metadataParsed,
                                                     "extrusion_distance_mm",
                                                     0.f);
            out->extrusion_distance_mm[1] = 0;
            out->extruder_temperature[0] = get_leaf(m_metadataParsed,
                                                    "extruder_temperature",
                                                    static_cast<int>(0));
            out->extruder_temperature[1] = 0;
            out->extruder_count = 1;
            const std::string material = get_leaf(m_metadataParsed,
                                                  "material", "UNKNOWN");
            if (material.size() >= MATERIAL_MAX_LENGTH) {
              return Error::kMaxStringLengthExceeded;
            }
            memset(out->material[0], 0, sizeof(char)*MATERIAL_MAX_LENGTH);
            material.copy(out->material[0], material.size());
            out->material[1][0] = 0;
            out->tool_type[0]
                = bwcoreutils::YonkersTool
                  ::type_from_type_name(get_leaf(m_metadataParsed,
                                                 "tool_type", "unknown"));
            out->tool_type[1] = bwcoreutils::TYPE::UNKNOWN_TYPE;
        } else if (m_metafileVersion.major == 3) {
            size_t extruders = std::max(2u,
                                        get_arr(m_metadataParsed,
                                                "extruder_temperatures")
                                        .size());
            out->extruder_count = extruders;
            for (size_t i = 0; i < extruders; i++) {
                Json::ArrayIndex ai(i);
                out->extrusion_mass_g[i]
                    = get_leaf(get_arr(m_metadataParsed, "extrusion_masses_g"),
                               ai, 0.f);
                out->extrusion_distance_mm[i]
                    = get_leaf(get_arr(m_metadataParsed,
                                       "extrusion_distances_mm"),
                               ai, 0.f);
                out->extruder_temperature[i]
                    = get_leaf(get_arr(m_metadataParsed,
                                       "extruder_temperatures"),
                               ai, static_cast<int>(0));
                const std::string material
                    = get_leaf(get_arr(m_metadataParsed, "materials"),
                               ai, "");
                if (material.size() >= MATERIAL_MAX_LENGTH) {
                    return Error::kMaxStringLengthExceeded;
                }
                memset(out->material[i], 0, sizeof(char)*MATERIAL_MAX_LENGTH);
                material.copy(out->material[i], material.size());
                out->tool_type[i]
                    = bwcoreutils::YonkersTool
                      ::type_from_type_name(get_leaf(get_arr(m_metadataParsed,
                                                             "tool_types"),
                                                     ai, "unknown"));
            }
        }
        // If you are trying to add a new version, please update
        // maxSupportedVersion
        out->duration_s = get_leaf(m_metadataParsed, "duration_s", 0.f);
        if (m_metadataParsed.isMember("build_plane_temperature")) {
             out->buildplane_target_temperature = get_leaf(m_metadataParsed,
                     "build_plane_temperature", static_cast<int>(0));
             out->chamber_temperature = chamberTempFromBuildplane(
                 out->buildplane_target_temperature);
        } else {
             out->chamber_temperature = get_leaf(m_metadataParsed,
                     "chamber_temperature", static_cast<int>(0));
             out->buildplane_target_temperature = buildplaneTempFromChamber(
                 out->chamber_temperature);
        }
        out->platform_temperature = get_leaf(m_metadataParsed,
            "platform_temperature", static_cast<int>(0));
        out->uses_raft = get_leaf(get_obj(m_metadataParsed, "miracle_config"),
                                  "doRaft", false);
        const std::string type = get_leaf(m_metadataParsed, "bot_type", "");
        out->bot_pid = parse_pid(type);
        out->override_tau_accel       = get_leaf(m_metadataParsed,"override_tau_accel",       0.f);
        out->override_tau_decel       = get_leaf(m_metadataParsed,"override_tau_decel",       0.f);
        out->override_winding_current = get_leaf(m_metadataParsed,"override_winding_current", 0.f);

        if (m_metafileVersion > SemVer(1, 0, 0)
            && m_metafileVersion < SemVer(3, 0, 0)) {
            out->bounding_box_x_min = get_leaf(m_metadataParsed,
                                               "bounding_box_x_min", 0.f);
            out->bounding_box_x_max = get_leaf(m_metadataParsed,
                                               "bounding_box_x_max", 0.f);
            out->bounding_box_y_min = get_leaf(m_metadataParsed,
                                               "bounding_box_y_min", 0.f);
            out->bounding_box_y_max = get_leaf(m_metadataParsed,
                                               "bounding_box_y_max", 0.f);
            out->bounding_box_z_min = get_leaf(m_metadataParsed,
                                               "bounding_box_z_min", 0.f);
            out->bounding_box_z_max = get_leaf(m_metadataParsed,
                                               "bounding_box_z_max", 0.f);
        } else if (m_metafileVersion >= SemVer(3, 0, 0)) {
            auto bounding_box = get_obj(m_metadataParsed, "bounding_box");
            out->bounding_box_x_max = get_leaf(bounding_box, "x_max", 0.f);
            out->bounding_box_x_min = get_leaf(bounding_box, "x_min", 0.f);
            out->bounding_box_y_max = get_leaf(bounding_box, "y_max", 0.f);
            out->bounding_box_y_min = get_leaf(bounding_box, "y_min", 0.f);
            out->bounding_box_z_max = get_leaf(bounding_box, "z_max", 0.f);
            out->bounding_box_z_min = get_leaf(bounding_box, "z_min", 0.f);
        }
    }

    // common output
    out->thing_id = get_leaf(m_metadataParsed, "thing_id", (unsigned int)0);
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
    const std::string uuid = get_leaf(m_metadataParsed, "uuid", "");
    if (uuid.size() > UUID_MAX_LENGTH) {
        return Error::kMaxStringLengthExceeded;
    }
    uuid.copy(out->uuid, uuid.size());

    return Error::kOK;
}

TinyThingReader::Error
TinyThingReader::Private::getCppOnlyMetadata(Metadata* out) const {
    using namespace MakerBot::SafeJson;  // NOLINT(build/namespaces)
    if (m_metafileVersion.major == -1) {
        return TinyThingReader::Error::kNotYetUnzipped;
    } else if (m_metafileVersion > maxSupportedVersion()) {
        return TinyThingReader::Error::kVersionMismatch;
    } else if (m_metafileVersion.major == 0) {
        const Json::Value printer_settings = get_obj(m_metadataParsed,
                                                     "printer_settings");
        out->shells = get_leaf(printer_settings, "shells", static_cast<int>(0));
        out->layer_height = get_leaf(printer_settings, "layer_height", 0.f);
        out->infill_density = get_leaf(printer_settings, "infill", 0.f);
        out->uses_support = get_leaf(printer_settings, "support", false);
        // Not included in this version
        out->max_flow_rate[0] = out->max_flow_rate[1] = 0.f;
        out->slicer_name = get_leaf(printer_settings, "slicer", "UNKNOWN");
    } else {
        const Json::Value miracle_config = get_obj(m_metadataParsed,
                                                   "miracle_config");
        out->shells = get_leaf(miracle_config, "numberOfShells",
                               static_cast<int>(0));
        out->layer_height = get_leaf(miracle_config, "layerHeight", 0.f);
        out->infill_density = get_leaf(miracle_config, "infillDensity", 0.f);
        out->uses_support = get_leaf(miracle_config, "doSupport", false);
        out->slicer_name = get_leaf(miracle_config, "slicer", "UNKNOWN");
        if (m_metafileVersion.major < 3) {
            out->max_flow_rate[0] = get_leaf(m_metadataParsed,
                                             "max_flow_rate",
                                             0.f);
            out->max_flow_rate[1] = 0;
        } else {
            out->max_flow_rate[0] = 0;
            out->max_flow_rate[1] = 0;
        }
    }
    // If you are adding a new version, please make sure to update
    // maxSupportedVersion
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

TinyThingReader::Error
TinyThingReader::Private::getPurgeRoutines(const char** out) const {
    if (m_purgeRoutineContents.empty()) {
        *out = nullptr;
        return TinyThingReader::kNotYetUnzipped;
    }
    *out = m_purgeRoutineContents.c_str();
    return TinyThingReader::kOK;
}

TinyThingReader::TinyThingReader(const std::string& filePath, int fd)
    : m_private(new Private(filePath, fd)) {}

TinyThingReader::~TinyThingReader() {
}

bool TinyThingReader::unzipMetadataFile() {
    using namespace MakerBot::SafeJson;  // NOLINT(build/namespaces)
    const bool unzipped
        = m_private->unzipFile(Config::kMetadataFilename,
                               m_private->m_metadataFileContents);
    if (!unzipped) {return unzipped;}
    const bool extracted
        = Private::parseFile(m_private->m_metadataFileContents,
                             &(m_private->m_metadataParsed));
    m_private->m_metafileVersion
        = SemVer(get_leaf(m_private->m_metadataParsed, "version", "0.0.0"));
    if (extracted) {
        auto fw = Json::FastWriter();
        m_private->m_sliceProfileContents
            = fw.write(get_obj(m_private->m_metadataParsed, "miracle_config"));
        m_private->m_purgeRoutineContents =
            fw.write(get_arr(m_private->m_metadataParsed, "purge_routines"));
    }
    return extracted;
}

bool TinyThingReader::unzipIsometricSmallThumbnailFile() {
    return m_private->unzipFile(
        Config::kIsometricSmallThumbnailFilename,
        m_private->m_isometricSmallThumbnailFileContents);
}

bool TinyThingReader::unzipIsometricMediumThumbnailFile() {
    return m_private->unzipFile(
        Config::kIsometricMediumThumbnailFilename,
        m_private->m_isometricMediumThumbnailFileContents);
}

bool TinyThingReader::unzipIsometricLargeThumbnailFile() {
    return m_private->unzipFile(
        Config::kIsometricLargeThumbnailFilename,
        m_private->m_isometricLargeThumbnailFileContents);
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

bool TinyThingReader::unzipSombreroSmallThumbnailFile() {
    return m_private->unzipFile(
        Config::kSombreroSmallThumbnailFilename,
        m_private->m_sombreroSmallThumbnailFileContents);
}

bool TinyThingReader::unzipSombreroMediumThumbnailFile() {
    return m_private->unzipFile(
        Config::kSombreroMediumThumbnailFilename,
        m_private->m_sombreroMediumThumbnailFileContents);
}

bool TinyThingReader::unzipSombreroLargeThumbnailFile() {
    return m_private->unzipFile(
        Config::kSombreroLargeThumbnailFilename,
        m_private->m_sombreroLargeThumbnailFileContents);
}

bool TinyThingReader::unzipSmallSquareThumbnailFile() {
    return m_private->unzipFile(
        Config::kSmallSquareThumbnailFilename,
        m_private->m_smallSquareThumbnailFileContents);
}

bool TinyThingReader::unzipToolpathFile() {
    m_private->m_incremental = false;
    // first try jsontoolpath
    bool ok = m_private->unzipFile(
            Config::kJsonToolpathFilename,
            m_private->m_toolpathFileContents);
    if (ok) {
        return m_private->resetToolpath();
    }

    ok = m_private->unzipFile(
            Config::kGcodeToolpathFilename,
            m_private->m_toolpathFileContents);

    if (ok) {
        return m_private->resetToolpath();
    }

    return ok;
}

TinyThingReader::Error
TinyThingReader::getSliceProfile(const char** out) const {
    return m_private->getSliceProfile(out);
}

TinyThingReader::Error
TinyThingReader::getPurgeRoutines(const char** out) const {
    return m_private->getPurgeRoutines(out);
}

TinyThingReader::Error TinyThingReader::getMetadata(Metadata* out) const {
    const Error error = m_private->getMetadata<Metadata>(out);
    if (error != kOK) {return error;}
    return m_private->getCppOnlyMetadata(out);
}

TinyThingReader::Error
TinyThingReader::getMetadata(CInterfaceMetadata* out) const {
    return m_private->getMetadata<CInterfaceMetadata>(out);
}

void
TinyThingReader::getIsometricSmallThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_isometricSmallThumbnailFileContents;
}

void
TinyThingReader::getIsometricMediumThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_isometricMediumThumbnailFileContents;
}

void
TinyThingReader::getIsometricLargeThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_isometricLargeThumbnailFileContents;
}

void
TinyThingReader::getSmallThumbnailFileContents(std::string* contents) const {
    *contents = m_private->m_smallThumbnailFileContents;
}

void
TinyThingReader::getMediumThumbnailFileContents(std::string* contents) const {
    *contents = m_private->m_mediumThumbnailFileContents;
}

void
TinyThingReader::getLargeThumbnailFileContents(std::string* contents) const {
    *contents = m_private->m_largeThumbnailFileContents;
}

void
TinyThingReader::getSombreroSmallThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_sombreroSmallThumbnailFileContents;
}

void
TinyThingReader::getSombreroMediumThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_sombreroMediumThumbnailFileContents;
}

void
TinyThingReader::getSombreroLargeThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_sombreroLargeThumbnailFileContents;
}

void
TinyThingReader::getSmallSquareThumbnailFileContents(
        std::string* contents) const {
    *contents = m_private->m_smallSquareThumbnailFileContents;
}


void TinyThingReader::getToolpathFileContents(std::string* contents) const {
    *contents = m_private->m_toolpathFileContents;
}

std::string TinyThingReader::getToolpathFileContents() const {
    return m_private->m_toolpathFileContents;
}

bool TinyThingReader::hasJsonToolpath() const {
    return m_private->hasJsonToolpath();
}

bool TinyThingReader::hasGcodeToolpath() const {
    return m_private->hasGcodeToolpath();
}

bool TinyThingReader::hasMetadata() const {
    return m_private->hasMetadata();
}

bool TinyThingReader::isValid() const {
    return m_private->isValid();
}

// returns true if succesful
bool TinyThingReader::resetToolpath() {
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
TinyThingReader::doesMetadataFileMatchConfig(
                                         const VerificationData& config) const {
    return m_private->verifyMetadata(config);
}
