
#ifndef TINYTHINGREADER_HH_
#define TINYTHINGREADER_HH_

#include <memory>
#include <string>

namespace LibTinyThing {

	class TINYTHING_API TinyThingReader {
	public:

        TinyThingReader(const std::string& filePath);

		~TinyThingReader();

		// these functions unzip the contents of each of these files,
		// and cache them in memory. they return true if the unzip is
		// succesful
		bool unzipMetadataFile();
		bool unzipSmallThumbnailFile();
		bool unzipMediumThumbnailFile();
		bool unzipLargeThumbnailFile();
		bool unzipToolpathFile();

		// these are accessors for the unzipped contents of each of the
		// files. they should only be called after each has been
		// unzipped
		std::string getMetadataFileContents();
		std::string getSmallThumbnailFileContents();
		std::string getMediumThumbnailFileContents();
		std::string getLargeThumbnailFileContents();
		std::string getToolpathFileContents();

		// checks to see if toolpath is present
		bool hasJsonToolpath();

		// checks to see if all files are present
		bool isValid();

        // incremental unzipping of toolpath
        bool resetToolpath();
        int getToolpathIncr(const int chars, std::string* output);

	private:
        class Private;
        std::unique_ptr<Private> m_private;
	};

}


#endif /* TINYTHINGREADER_HH_ */
