
#ifndef TINYTHINGREADER_HH_
#define TINYTHINGREADER_HH_

#include <string>
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

namespace LibTinyThing {

	class TinyThingReader {
	public:

		TinyThingReader(const std::string& filePath) : m_filePath(filePath),
			m_toolpathFile(NULL), m_toolpathSize(0), m_toolpathPos(0) {}

		~TinyThingReader();

		// these functions unzip the contents of each of these files,
		// and cache them in memory. they return true if the unzip is
		// succesful 
		bool unzipMetadataFile();
		bool unzipThumbnailFile();
		bool unzipToolpathFile();


		// these are accessors for the unzipped contents of each of the 
		// files. they should only be called after each has been
		// unzipped 
		std::string getMetadataFileContents();
		std::string getThumbnailFileContents();
		std::string getToolpathFileContents();

		// checks to see if toolpath is present
		bool hasJsonToolpath();

		// checks to see if all files are present
		bool isValid();

        // incremental unzipping of toolpath
        bool resetToolpath();
        std::string getToolpathIncr(const int chars);

	private:

		bool unzipFile(const std::string& fileName,
				  	   std::string &output);

		const std::string m_filePath;

		std::string m_toolpathFileContents;
		std::string m_thumbnailFileContents;
		std::string m_metadataFileContents;

        // variables to support incremental toolpath unzipping
        unzFile m_toolpathFile;
        int m_toolpathSize;
        int m_toolpathPos;
	};

}


#endif /* TINYTHINGREADER_HH_ */




