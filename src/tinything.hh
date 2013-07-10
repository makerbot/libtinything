
#ifndef TINYTHING_HH_
#define TINYTHING_HH_

#include <string>
#include "miniunzip/unzip.h"
#include "miniunzip/zip.h"

namespace LibTinyThing {

	class TinyThing {
	public:

		TinyThing(const std::string& filePath) : m_filePath(filePath) {}

		~TinyThing();

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

		// these functions add a file into the tinything,
		// but need some work 
		bool addThumbnailFile(const std::string& filePath);
		bool addToolpathFile(const std::string& filePath);
		bool addMetadataFile(const std::string& filePath);

		// checks to see if all files are present
		// TODO: implement multiple levels of validity?
		bool isValid();

        // incremental unzipping of toolpath
        bool resetToolpath();
        std::string getToolpathIncr(const int chars);

	private:

		bool addFile(const std::string& fileName, const std::string& filePath);

		bool unzipFile(const std::string& fileName,
				  	   std::string &output);

		const std::string m_filePath;

		std::string m_toolpathFileContents;
		std::string m_thumbnailFileContents;
		std::string m_metadataFileContents;

		static const std::string METADATA_FILENAME;
		static const std::string THUMBNAIL_FILENAME;
		static const std::string TOOLPATH_FILENAME;

        // variables to support incremental toolpath unzipping
        unzFile m_toolpathFile;
        int m_toolpathSize;
        int m_toolpathPos;
	};

}


#endif /* TINYTHING_HH_ */




