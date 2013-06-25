#include "tinything.hh"
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>

namespace LibTinyThing {

	bool TinyThing::unzipMetadataFile() {
		return unzipFile(METADATA_FILENAME, m_metadataFileContents);
	}

	bool TinyThing::unzipThumbnailFile() {
		return unzipFile(THUMBNAIL_FILENAME, m_thumbnailFileContents);
	}

	bool TinyThing::unzipToolpathFile() {
		return unzipFile(TOOLPATH_FILENAME, m_toolpathFileContents);
	}

	std::string TinyThing::getMetadataFileContents(){
		return m_metadataFileContents;
	}
	std::string TinyThing::getThumbnailFileContents(){
		return m_thumbnailFileContents;
	}
	std::string TinyThing::getToolpathFileContents(){
		return m_toolpathFileContents;
	}

	bool TinyThing::isValid(){
		unzFile tinyThingFile = unzOpen(m_filePath.c_str());

		bool isValid = (tinyThingFile != NULL &&
				unzLocateFile(tinyThingFile, METADATA_FILENAME.c_str(), 0) == UNZ_OK &&
				unzLocateFile(tinyThingFile, THUMBNAIL_FILENAME.c_str(), 0) == UNZ_OK &&
				unzLocateFile(tinyThingFile, TOOLPATH_FILENAME.c_str(), 0) == UNZ_OK &&
				unzOpenCurrentFile(tinyThingFile) == UNZ_OK);

		if (tinyThingFile != NULL)
			unzClose(tinyThingFile);

		return isValid;
	}


	bool TinyThing::unzipFile(const std::string& fileName,
				   std::string &output) {

		unzFile tinyThingFile = unzOpen(m_filePath.c_str());

		if (tinyThingFile != NULL   &&
			unzLocateFile(tinyThingFile, fileName.c_str(), 0) == UNZ_OK &&
			unzOpenCurrentFile(tinyThingFile) == UNZ_OK) {

			unz_file_info fileInfo;

			if (unzGetCurrentFileInfo(tinyThingFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {

				// Create a buffer large enough to hold the uncompressed thumbnail
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

	bool TinyThing::addMetadataFile(const std::string& filePath){
		return addFile(METADATA_FILENAME, filePath);
	}

	bool TinyThing::addThumbnailFile(const std::string& filePath){
		return addFile(THUMBNAIL_FILENAME, filePath);
	}

	bool TinyThing::addToolpathFile(const std::string& filePath){
		return addFile(TOOLPATH_FILENAME, filePath);
	}

	bool TinyThing::addFile(const std::string& fileName,
				 const std::string& filePath) {

		std::cout << "Adding " << filePath << " to " << m_filePath << " as " << fileName << std::endl;

		// TODO: figure out how to handle the various cases of pre-existing files
		int append_status = APPEND_STATUS_CREATE;
	    std::ifstream zipfile(m_filePath.c_str()); // std::ios::binary
	    if (zipfile) {
	    	std::cout << "APPEND STATUS ADD IN ZIP";
	    	append_status = APPEND_STATUS_ADDINZIP;
	    }

		zipFile zip(zipOpen(m_filePath.c_str(), append_status));
		if (!zip)
			return false;

		std::cout << "Opened!" << std::endl;

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

		zipOpenNewFileInZip(zip, fileName.c_str(), &zi, NULL, 0, NULL, 0, NULL,
							Z_DEFLATED, Z_DEFAULT_COMPRESSION);

		// TODO: dont load entire file into memory

		std::ifstream file(filePath.c_str(), std::ios::in|std::ios::ate); // std::ios::binary
		if (!file.is_open())
			return false;

		std::ifstream::pos_type size = file.tellg();

		std::vector<char> memblock;
		memblock.resize(size);
		//char* memblock = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(memblock.data(), size);
		file.close();

		//delete[] memblock;

		zipWriteInFileInZip(zip, memblock.data(), size);

		zipClose(zip, "MakerBot Tinything file");
		return true;

	}

	const std::string TinyThing::METADATA_FILENAME = "meta.json";;
	const std::string TinyThing::THUMBNAIL_FILENAME = "thumbnail.png";
	const std::string TinyThing::TOOLPATH_FILENAME = "print.jsontoolpath";


}
