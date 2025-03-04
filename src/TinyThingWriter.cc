
#include <ctime>
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "tinything/TinyThingWriter.hh"
#include "tinything/TinyThingConstants.hh"
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

namespace LibTinyThing {

class TinyThingWriter::Private {
public:
    Private(const std::string &filePath) :
        m_zipFilePath(filePath) {
    }

    bool addFile(const std::string& fileName,
                 const std::string& filePath,
                 bool compress, bool createZip) {

        // std::cout << "INFO: Adding " << filePath << " to "
        //     << m_zipFilePath << " as " << fileName << std::endl;

        int append_status = APPEND_STATUS_ADDINZIP;
        if (createZip) {
            append_status = APPEND_STATUS_CREATE;
        }

        zipFile zip(zipOpen(m_zipFilePath.c_str(), append_status));
        if (!zip)
            return false;

        zip_fileinfo zi;

        // Set date and time
        const time_t curTime(time(NULL));
        struct tm localTime;
        localTime = *localtime(&curTime);
        zi.tmz_date.tm_sec = localTime.tm_sec;
        zi.tmz_date.tm_min = localTime.tm_min;
        zi.tmz_date.tm_hour = localTime.tm_hour;
        zi.tmz_date.tm_mday = localTime.tm_mday;
        zi.tmz_date.tm_mon = localTime.tm_mon;
        zi.tmz_date.tm_year = localTime.tm_year;

        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;

        const int compression = compress ? Z_DEFAULT_COMPRESSION : 0;

        // TODO(pshaw): dont load entire file into memory
        std::ifstream file(filePath.c_str(), 
            std::ios::in | std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            zipClose(zip, "MakerBot file");
            return false;
        }

        zipOpenNewFileInZip(zip, fileName.c_str(), &zi,
                            NULL, 0, NULL, 0, NULL,
                            Z_DEFLATED, compression);

        std::ifstream::pos_type size = file.tellg();

        std::vector<char> memblock;
        memblock.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(memblock.data(), size);
        file.close();

        zipWriteInFileInZip(zip, memblock.data(), size);

        zipClose(zip, "MakerBot file");
        return true;
    }

    bool zip() {
        // toolpath file is required
        if(m_toolpathFilePath.empty()){
            std::cout << "ERROR: No toolpath file" << std::endl;
            return false;
        }
        const std::string gcode_ending = ".gcode";
        int expected_extension_idx =
                m_toolpathFilePath.size() - gcode_ending.size();
        bool is_gcode_toolpath =
            m_toolpathFilePath.rfind(gcode_ending) == expected_extension_idx;

        if (!addFile(
                is_gcode_toolpath ?
                    Config::kGcodeToolpathFilename :
                    Config::kJsonToolpathFilename,
                m_toolpathFilePath,
                true,
                true)) {
            return false;
        }

        // add metadata if it is set
        if (!m_metadataFilePath.empty()){
            if(!addFile(Config::kMetadataFilename, m_metadataFilePath, false, false)){
                std::cout << "ERROR: could not add metadata" << std::endl;
                return false;
            }
        } else {
            std::cout << "WARNING: skipping metadata, not specified" << std::endl;
        }

        // add thumbnails if path is set
        if (!m_thumbnailDirPath.empty()){
            int thumbnailCount = 0;
            bool gotSquareThumbnail = false;
            // try to add isometric images, which should work for all bots
            if(addFile(Config::kIsometricSmallThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kIsometricSmallThumbnailFilename,
                        false, false)){
                std::cout << "Adding small isometric thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kIsometricMediumThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kIsometricMediumThumbnailFilename,
                        false, false)){
                std::cout << "Adding medium isometric thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kIsometricLargeThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kIsometricLargeThumbnailFilename,
                        false, false)){
                std::cout << "Adding large isometric thumbnail" << std::endl;
                thumbnailCount++;
            }
            // try to add birdwing-style images, which should only work for birdwing bots
            if(addFile(Config::kSmallThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kSmallThumbnailFilename,
                        false, false)){
                std::cout << "Adding small thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kMediumThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kMediumThumbnailFilename,
                        false, false)){
                std::cout << "Adding medium thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kLargeThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kLargeThumbnailFilename,
                        false, false)){
                std::cout << "Adding large thumbnail" << std::endl;
                thumbnailCount++;
            }
            // try to add sombrero-style images, which should only work for sombrero bots
            if(addFile(Config::kSombreroSmallThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kSombreroSmallThumbnailFilename,
                        false, false)){
                std::cout << "Adding small thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kSombreroMediumThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kSombreroMediumThumbnailFilename,
                        false, false)){
                std::cout << "Adding medium thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kSombreroLargeThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kSombreroLargeThumbnailFilename,
                        false, false)){
                std::cout << "Adding large thumbnail" << std::endl;
                thumbnailCount++;
            }
            if(addFile(Config::kSmallSquareThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kSmallSquareThumbnailFilename,
                        false, false)){
                std::cout << "Adding small square thumbnail" << std::endl;
                gotSquareThumbnail = true;
                thumbnailCount++;
            }
            // Sketch is currently the only bot that uses the square thumbnail,
            // and the 90x90 one is the only thumbnail (of the non-isometric
            // ones) that it uses
            if (gotSquareThumbnail) {
                 if (thumbnailCount != 4) {
                     std::cout << "ERROR: Expected to add 4 thumbnail images; got " << thumbnailCount << "." << std::endl;
                     return false;
                 }
            } else if (thumbnailCount != 6) {
                 std::cout << "ERROR: Expected to add 6 thumbnail images; got " << thumbnailCount << "." << std::endl;
                 return false;
            }
        } else {
            // std::cout << "WARNING: Skipping thumbnails, not specified" << std::endl;
        }

        return true;
    }

    const std::string m_zipFilePath;
    std::string m_metadataFilePath;
    std::string m_toolpathFilePath;
    std::string m_thumbnailDirPath;
};

TinyThingWriter::TinyThingWriter(const std::string& filePath) :
    m_private(new Private(filePath)) {
}

TinyThingWriter::~TinyThingWriter() {
}

void TinyThingWriter::setMetadataFile(const std::string& filePath){
    m_private->m_metadataFilePath = filePath;
}

void TinyThingWriter::setThumbnailDirectory(const std::string& dirPath){
    m_private->m_thumbnailDirPath = dirPath;
}

void TinyThingWriter::setToolpathFile(const std::string& filePath){
    m_private->m_toolpathFilePath = filePath;
}

bool TinyThingWriter::zip(){
    return m_private->zip();
}
}
