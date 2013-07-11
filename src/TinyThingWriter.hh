
#ifndef TINYTHINGWRITER_HH_
#define TINYTHINGWRITER_HH_

#include <string>
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

namespace LibTinyThing {

	class TinyThingWriter {
	public:

		TinyThingWriter(const std::string& filePath) : m_filePath(filePath) {}

		~TinyThingWriter();

		// these functions add a file into the tinything,
		// but need some work 
		bool addThumbnailFile(const std::string& filePath);
		bool addToolpathFile(const std::string& filePath);
		bool addMetadataFile(const std::string& filePath);

		static const std::string METADATA_FILENAME;
		static const std::string THUMBNAIL_FILENAME;
		static const std::string TOOLPATH_FILENAME;

	private:

		const std::string m_filePath;

		bool addFile(const std::string& fileName, 
                     const std::string& filePath, 
                     bool createNew = 1);



	};

}


#endif /* TINYTHINGWRITER_HH_ */




