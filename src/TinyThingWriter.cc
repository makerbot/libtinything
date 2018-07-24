
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

        zipOpenNewFileInZip(zip, fileName.c_str(), &zi,
                            NULL, 0, NULL, 0, NULL,
                            Z_DEFLATED, compression);

        // TODO(pshaw): dont load entire file into memory
        std::ifstream file(filePath.c_str(), 
            std::ios::in | std::ios::ate | std::ios::binary);
        if (!file.is_open())
            return false;

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
        if (!addFile(Config::kToolpathFilename, m_toolpathFilePath, true, true))
            return false;

        // add metadata if it is set
        if (!m_metadataFilePath.empty()){
            if(!addFile(Config::kMetadataFilename, m_metadataFilePath, false, false)){
                std::cout << "ERROR: could not add metadata" << std::endl;
                return false;
            }
        } else {
            std::cout << "WARNING: skipping metadata, not specified" << std::endl;
        }

        // add metadata if it is set
        if (!m_thumbnailDirPath.empty()){
            if(!addFile(Config::kSmallThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kSmallThumbnailFilename,
                        false, false)){
                std::cout << "ERROR: could not add small thumbnail" << std::endl;
                return false;
            }
            if(!addFile(Config::kMediumThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kMediumThumbnailFilename,
                        false, false)){
                std::cout << "ERROR: could not add medium thumbnail" << std::endl;
                return false;
            }
            if(!addFile(Config::kLargeThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kLargeThumbnailFilename,
                        false, false)){
                std::cout << "ERROR: could not add large thumbnail" << std::endl;
                return false;
            }
            if(!addFile(Config::kFullViewSmallThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kFullViewSmallThumbnailFilename,
                        false, false)){
                std::cout << "ERROR: could not add small full-view thumbnail"<< Config::kFullViewSmallThumbnailFilename << std::endl;
            }
            if(!addFile(Config::kFullViewMediumThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kFullViewMediumThumbnailFilename,
                        false, false)){
                std::cout << "ERROR: could not add medium full-view thumbnail" << Config::kFullViewSmallThumbnailFilename << std::endl;
            }
            if(!addFile(Config::kFullViewLargeThumbnailFilename,
                        m_thumbnailDirPath + "/" + Config::kFullViewLargeThumbnailFilename,
                        false, false)){
                std::cout << "ERROR: could not add large full-view thumbnail" << Config::kFullViewSmallThumbnailFilename << std::endl;
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
