
#ifndef TINYTHINGWRITER_HH_
#define TINYTHINGWRITER_HH_

#include <string>
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

namespace LibTinyThing {

	class TinyThingWriter {
	public:

		TinyThingWriter(const std::string& filePath) : m_filePath(filePath),
			m_metadataFilePath(""),
			m_toolpathFilePath(""),
			m_thumbnailFilePath("") {}

		~TinyThingWriter() {};

		void setThumbnailFile(const std::string& filePath);
		void setToolpathFile(const std::string& filePath);
		void setMetadataFile(const std::string& filePath);

		bool zip();

		static const std::string METADATA_FILENAME;
		static const std::string THUMBNAIL_FILENAME;
		static const std::string TOOLPATH_FILENAME;

	private:

		const std::string m_filePath;
		std::string m_metadataFilePath;
		std::string m_toolpathFilePath;
		std::string m_thumbnailFilePath;

		bool addFile(const std::string& fileName, 
                     const std::string& filePath, 
                     bool createNew);



	};

}


#endif /* TINYTHINGWRITER_HH_ */




