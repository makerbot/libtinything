#include "tinything.hh"

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

	const std::string TinyThing::METADATA_FILENAME = "meta.json";;
	const std::string TinyThing::THUMBNAIL_FILENAME = "thumbnail.png";
	const std::string TinyThing::TOOLPATH_FILENAME = "print.jsontoolpath";


}
