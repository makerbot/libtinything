
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>

#include "tinything/TinyThingReader.hh"
#include "tinything/TinyThingConstants.hh"
#include "tinything/unzip.h"
#include "tinything/zip.h"

namespace LibTinyThing {
class TinyThingReader::Private {
 public:
  Private(const std::string& filePath)
      : m_filePath(filePath),
        m_toolpathFile(NULL),
        m_toolpathSize(0),
        m_toolpathPos(0) {
  }

  bool unzipFile(
      const std::string& fileName,
      std::string &output) {

    unzFile tinyThingFile = unzOpen(m_filePath.c_str());
    bool retval = false;

    if (tinyThingFile != NULL   &&
        unzLocateFile(tinyThingFile, fileName.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(tinyThingFile) == UNZ_OK) {

      unz_file_info fileInfo;

      if (unzGetCurrentFileInfo(tinyThingFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {

        // Create a buffer large enough to hold the uncompressed file
        output.resize(fileInfo.uncompressed_size);

        if (unzReadCurrentFile(tinyThingFile,
                               const_cast<char*>(output.c_str()),
                               fileInfo.uncompressed_size)) {
          retval = true;
        }
      }
    }

    // Cleanup, if necessary
    if (tinyThingFile != NULL)
      unzClose(tinyThingFile);
    return retval;
  }

  const std::string m_filePath;

  std::string m_toolpathFileContents;
  std::string m_smallThumbnailFileContents;
  std::string m_mediumThumbnailFileContents;
  std::string m_largeThumbnailFileContents;
  std::string m_metadataFileContents;

  // variables to support incremental toolpath unzipping
  unzFile m_toolpathFile;
  int m_toolpathSize;
  int m_toolpathPos;
};

TinyThingReader::TinyThingReader(const std::string& filePath)
    : m_private(new Private(filePath)) {
}

TinyThingReader::~TinyThingReader() {
    if (m_private->m_toolpathFile != NULL)
        unzClose(m_private->m_toolpathFile);
}

bool TinyThingReader::unzipMetadataFile() {
    return m_private->unzipFile(
        Config::kMetadataFilename, m_private->m_metadataFileContents);
}

bool TinyThingReader::unzipSmallThumbnailFile() {
    return m_private->unzipFile(
        Config::kSmallThumbnailFilename, m_private->m_smallThumbnailFileContents);
}

bool TinyThingReader::unzipMediumThumbnailFile() {
    return m_private->unzipFile(
        Config::kMediumThumbnailFilename, m_private->m_mediumThumbnailFileContents);
}

bool TinyThingReader::unzipLargeThumbnailFile() {
    return m_private->unzipFile(
        Config::kLargeThumbnailFilename, m_private->m_largeThumbnailFileContents);
}

bool TinyThingReader::unzipToolpathFile() {
    return m_private->unzipFile(
        Config::kToolpathFilename, m_private->m_toolpathFileContents);
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
    unzFile tinyThingFile = unzOpen(m_private->m_filePath.c_str());

    bool isValid = (
        tinyThingFile != NULL &&
        unzLocateFile(tinyThingFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(tinyThingFile) == UNZ_OK);

    if (tinyThingFile != NULL)
        unzClose(tinyThingFile);

    return isValid;
}

bool TinyThingReader::isValid(){
    unzFile tinyThingFile = unzOpen(m_private->m_filePath.c_str());

    bool isValid = (
        tinyThingFile != NULL &&
        unzLocateFile(tinyThingFile, Config::kMetadataFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(tinyThingFile, Config::kSmallThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(tinyThingFile, Config::kMediumThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(tinyThingFile, Config::kSmallThumbnailFilename.c_str(), 0) == UNZ_OK &&
        unzLocateFile(tinyThingFile, Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(tinyThingFile) == UNZ_OK);

    if (tinyThingFile != NULL)
        unzClose(tinyThingFile);

    return isValid;
}

// returns true if succesful
bool TinyThingReader::resetToolpath(){

    m_private->m_toolpathPos = 0;

    if (m_private->m_toolpathFile != NULL)
        unzClose(m_private->m_toolpathFile);

    m_private->m_toolpathFile = unzOpen(m_private->m_filePath.c_str());

    if (m_private->m_toolpathFile != NULL &&
        unzLocateFile(m_private->m_toolpathFile,
            Config::kToolpathFilename.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(m_private->m_toolpathFile) == UNZ_OK) {

        unz_file_info fileInfo;

        if (unzGetCurrentFileInfo(m_private->m_toolpathFile, &fileInfo,
                                  NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
            m_private->m_toolpathSize = fileInfo.uncompressed_size;
            return true;
        }
    }
    return false;
}

// if length of return string is < bytes, you have reached end of file
std::string TinyThingReader::getToolpathIncr(const int chars) {

    if (m_private->m_toolpathFile == NULL)
        resetToolpath();

    int chars_to_read = chars;
    if (m_private->m_toolpathPos >= m_private->m_toolpathSize) {
        // at end of file
        return "";
    } else if ((m_private->m_toolpathPos + chars) > m_private->m_toolpathSize) {
        // reached end of file
        chars_to_read = m_private->m_toolpathSize - m_private->m_toolpathPos;
    }

    m_private->m_toolpathPos += chars_to_read;

    std::string output;
    output.resize(chars_to_read);

    unzReadCurrentFile(m_private->m_toolpathFile,
        const_cast<char*>(output.c_str()),
        chars_to_read);
    return output;
}
}
