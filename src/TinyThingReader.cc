
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>

#include "tinything/TinyThingReader.hh"
#include "tinything/TinyThingConstants.hh"
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

namespace LibTinyThing {
class TinyThingReader::Private {
 public:
  Private(const std::string& filePath, int fd)
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

  ~Private() {
    if (m_zipFile != NULL) {
      unzClose(m_zipFile);
    }
  }

  bool hasJsonToolpath() {
    m_incremental = false;
    bool hasToolpath = (
        m_zipFile != NULL &&
        unzLocateFile(m_zipFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(m_zipFile) == UNZ_OK);
    return hasToolpath;
  }

  bool isValid() {
    m_incremental = false;
    bool isValid = (
        m_zipFile != NULL &&
        unzLocateFile(m_zipFile, Config::kMetadataFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kSmallThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kMediumThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kSmallThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(m_zipFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(m_zipFile) == UNZ_OK);
    return isValid;
  }

  bool resetToolpath() {
    m_toolpathPos = 0;
    unz_file_info fileInfo;
    if (hasJsonToolpath() &&
        unzLocateFile(m_zipFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzGetCurrentFileInfo(m_zipFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
      m_toolpathSize = fileInfo.uncompressed_size;
      m_incremental = true;
      std::cout << "size: " << m_toolpathSize << std::endl;
      return true;
    } else {
      return false;
    }
  }

  std::string getToolpathIncr(const int chars) {
    if (!m_incremental) {
      resetToolpath();
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

  bool unzipFile(const std::string& fileName, std::string &output) {
    m_incremental = false;
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

  const std::string m_filePath;

  std::string m_toolpathFileContents;
  std::string m_smallThumbnailFileContents;
  std::string m_mediumThumbnailFileContents;
  std::string m_largeThumbnailFileContents;
  std::string m_metadataFileContents;

  // variables to support incremental toolpath unzipping
  unzFile m_zipFile;
  bool m_incremental; // are we set to incrementally unzip
  unsigned int m_toolpathSize;
  unsigned int m_toolpathPos;
};

TinyThingReader::TinyThingReader(const std::string& filePath, int fd)
    : m_private(new Private(filePath, fd)) {
}

TinyThingReader::~TinyThingReader() {
}

bool TinyThingReader::unzipMetadataFile() {
    return m_private->unzipFile(
        Config::kMetadataFilename, 
        m_private->m_metadataFileContents);
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
    return m_private->unzipFile(
        Config::kToolpathFilename, 
        m_private->m_toolpathFileContents);
}

std::string TinyThingReader::getMetadataFileContents(){
    return m_private->m_metadataFileContents;
}

std::string TinyThingReader::getSmallThumbnailFileContents(){
    return m_private->m_smallThumbnailFileContents;
}

std::string TinyThingReader::getMediumThumbnailFileContents(){
    return m_private->m_mediumThumbnailFileContents;
}

std::string TinyThingReader::getLargeThumbnailFileContents(){
    return m_private->m_largeThumbnailFileContents;
}

std::string TinyThingReader::getToolpathFileContents(){
    return m_private->m_toolpathFileContents;
}

bool TinyThingReader::hasJsonToolpath(){
  return m_private->hasJsonToolpath();
}

bool TinyThingReader::isValid(){
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
}
