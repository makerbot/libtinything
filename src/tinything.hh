
#ifndef TINYTHING_HH_
#define TINYTHING_HH_

#include <string>
#include "miniunzip/unzip.h"

namespace LibTinyThing {

	class TinyThing {
	public:

		TinyThing(const std::string& filePath) : m_filePath(filePath) {}

		~TinyThing() { }

		bool unzipMetadataFile();
		bool unzipThumbnailFile();
		bool unzipToolpathFile();

		std::string getMetadataFileContents();
		std::string getThumbnailFileContents();
		std::string getToolpathFileContents();

		// checks to see if all files are present
		bool isValid();

	private:

		bool unzipFile(const std::string& fileName,
				  	   std::string &output);

		const std::string m_filePath;

		std::string m_toolpathFileContents;
		std::string m_thumbnailFileContents;
		std::string m_metadataFileContents;

		static const std::string METADATA_FILENAME;
		static const std::string THUMBNAIL_FILENAME;
		static const std::string TOOLPATH_FILENAME;


	};

}


#endif /* TINYTHING_HH_ */




