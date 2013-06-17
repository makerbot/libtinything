/*
 * tinything.hh
 *
 *  Created on: Apr 25, 2013
 *      Author: paul
 */

#ifndef TINYTHING_HH_
#define TINYTHING_HH_

#include <string>
#include <common/miniunzip/unzip.h>

namespace LibTinyThing {

	class TinyThing {
	public:

		TinyThing(const std::string& filePath) : m_filePath(filePath) {}

		~TinyThing() { }

		bool unzipMetadataFile() {
			return unzipFile(METADATA_FILENAME, m_metadataFileContents);
		}

		bool unzipThumbnailFile() {
			return unzipFile(THUMBNAIL_FILENAME, m_thumbnailFileContents);
		}

		bool unzipToolpathFile() {
			return unzipFile(TOOLPATH_FILENAME, m_toolpathFileContents);
		}

		std::string getMetadataFileContents(){
			return m_metadataFileContents;
		}
		std::string getThumbnailFileContents(){
			return m_thumbnailFileContents;
		}
		std::string getToolpathFileContents(){
			return m_toolpathFileContents;
		}

		// checks to see if all files are present
		bool isValid(){
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

	private:

		bool unzipFile(const std::string& fileName,
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

		const std::string m_filePath;

		std::string m_toolpathFileContents;
		std::string m_thumbnailFileContents;
		std::string m_metadataFileContents;

		const std::string METADATA_FILENAME = "meta.json";;
		const std::string THUMBNAIL_FILENAME = "thumbnail.png";
		const std::string TOOLPATH_FILENAME = "print.jsontoolpath";


	};

}


#endif /* TINYTHING_HH_ */




