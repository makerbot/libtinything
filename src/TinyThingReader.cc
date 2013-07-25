#include "TinyThingReader.hh"
#include "TinyThingWriter.hh"
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>

namespace LibTinyThing {

    TinyThingReader::~TinyThingReader() {
	    if (m_toolpathFile != NULL) unzClose(m_toolpathFile);
    }

	bool TinyThingReader::unzipMetadataFile() {
		return unzipFile(TinyThingWriter::METADATA_FILENAME, m_metadataFileContents);
	}

	bool TinyThingReader::unzipThumbnailFile() {
		return unzipFile(TinyThingWriter::THUMBNAIL_FILENAME, m_thumbnailFileContents);
	}

	bool TinyThingReader::unzipToolpathFile() {
		return unzipFile(TinyThingWriter::TOOLPATH_FILENAME, m_toolpathFileContents);
	}

	std::string TinyThingReader::getMetadataFileContents(){    
		return m_metadataFileContents;
	}
	std::string TinyThingReader::getThumbnailFileContents(){
		return m_thumbnailFileContents;
	}
	std::string TinyThingReader::getToolpathFileContents(){
		return m_toolpathFileContents;
	}

	bool TinyThingReader::isValid(){
		unzFile tinyThingFile = unzOpen(m_filePath.c_str());

		bool isValid = (tinyThingFile != NULL &&
				unzLocateFile(tinyThingFile, TinyThingWriter::METADATA_FILENAME.c_str(), 0) == UNZ_OK &&
				unzLocateFile(tinyThingFile, TinyThingWriter::THUMBNAIL_FILENAME.c_str(), 0) == UNZ_OK &&
				unzLocateFile(tinyThingFile, TinyThingWriter::TOOLPATH_FILENAME.c_str(), 0) == UNZ_OK &&
				unzOpenCurrentFile(tinyThingFile) == UNZ_OK);

		if (tinyThingFile != NULL)
			unzClose(tinyThingFile);

		return isValid;
	}


	bool TinyThingReader::unzipFile(const std::string& fileName,
				   std::string &output) {

		unzFile tinyThingFile = unzOpen(m_filePath.c_str());

		if (tinyThingFile != NULL   &&
			unzLocateFile(tinyThingFile, fileName.c_str(), 0) == UNZ_OK &&
			unzOpenCurrentFile(tinyThingFile) == UNZ_OK) {

			unz_file_info fileInfo;

			if (unzGetCurrentFileInfo(tinyThingFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {

				// Create a buffer large enough to hold the uncompressed file
				output.resize(fileInfo.uncompressed_size);

				if (unzReadCurrentFile(tinyThingFile,
						const_cast<char*>(output.c_str()),
						fileInfo.uncompressed_size)){
					return true;
				}
			}
		}

		// Cleanup, if necessary
		if (tinyThingFile != NULL)
			unzClose(tinyThingFile);
		return false;

	}


    // returns true if succesful
    bool TinyThingReader::resetToolpath(){

        m_toolpathPos = 0;

        if (m_toolpathFile != NULL) unzClose(m_toolpathFile);

        m_toolpathFile = unzOpen(m_filePath.c_str());

		if (m_toolpathFile != NULL   &&
			unzLocateFile(m_toolpathFile, TinyThingWriter::TOOLPATH_FILENAME.c_str(), 0) == UNZ_OK &&
			unzOpenCurrentFile(m_toolpathFile) == UNZ_OK) {

			unz_file_info fileInfo;

			if (unzGetCurrentFileInfo(m_toolpathFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
                m_toolpathSize = fileInfo.uncompressed_size;
                return true;
            }
        }
        return false;        
    }

    // if length of return string is < bytes, you have reached end of file
    std::string TinyThingReader::getToolpathIncr(const int chars) {

        if (m_toolpathFile == NULL) resetToolpath();

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

		unzReadCurrentFile(m_toolpathFile,
			const_cast<char*>(output.c_str()),
			chars_to_read);
        return output;
	}




}
