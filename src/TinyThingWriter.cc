#include "TinyThingWriter.hh"
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

namespace LibTinyThing {


	void TinyThingWriter::setMetadataFile(const std::string& filePath){
		m_metadataFilePath = filePath;
	}

	void TinyThingWriter::setThumbnailFile(const std::string& filePath){
		m_thumbnailFilePath = filePath;
	}

	void TinyThingWriter::setToolpathFile(const std::string& filePath){
		m_toolpathFilePath = filePath;
	}

	bool TinyThingWriter::zip(){

		// we need a toolpath file
		if(m_toolpathFilePath.empty()){
			std::cout << "ERROR: No toolpath file" << std::endl;
			return false;
		}
		if (!addFile(TOOLPATH_FILENAME, m_toolpathFilePath, true))
			return false;


		// add metadata if it is set
		if (!m_metadataFilePath.empty()){
			if(!addFile(METADATA_FILENAME, m_metadataFilePath, false)){
				std::cout << "could not add metadata" << std::endl;
				return false;
			}
		} else {
			std::cout << "skipping metadata, not specified" << std::endl;
		}

		// add metadata if it is set
		if (!m_toolpathFilePath.empty()){
			if(!addFile(THUMBNAIL_FILENAME, m_thumbnailFilePath, false)){
				std::cout << "could not add thumbnail" << std::endl;
				return false;
			}
		} else {
			std::cout << "skipping thumbnail, not specified" << std::endl;
		}

		return true;

	}



	bool TinyThingWriter::addFile(const std::string& fileName,
				 const std::string& filePath, bool createZip) {

		std::cout << "Adding " << filePath << " to " << m_filePath << " as " << fileName << std::endl;

		int append_status = APPEND_STATUS_ADDINZIP;
	    if (createZip) {
	    	append_status = APPEND_STATUS_CREATE;
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


	const std::string TinyThingWriter::METADATA_FILENAME = "meta.json";
	const std::string TinyThingWriter::THUMBNAIL_FILENAME = "thumbnail.png";
	const std::string TinyThingWriter::TOOLPATH_FILENAME = "print.jsontoolpath";

}
