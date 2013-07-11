#include "TinyThingWriter.hh"
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>

namespace LibTinyThing {


	bool TinyThingWriter::addMetadataFile(const std::string& filePath){
		return addFile(METADATA_FILENAME, filePath);
	}

	bool TinyThingWriter::addThumbnailFile(const std::string& filePath){
		return addFile(THUMBNAIL_FILENAME, filePath);
	}

	bool TinyThingWriter::addToolpathFile(const std::string& filePath){
		return addFile(TOOLPATH_FILENAME, filePath);
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

    //bool zip(){
    //}

	const std::string TinyThingWriter::METADATA_FILENAME = "meta.json";
	const std::string TinyThingWriter::THUMBNAIL_FILENAME = "thumbnail.png";
	const std::string TinyThingWriter::TOOLPATH_FILENAME = "print.jsontoolpath";

}
