
#include "tinything/TinyThingConstants.hh"

namespace LibTinyThing {
    // TODO(pshaw): defining values here to get past linker errors
    // would be ideal to define these in a .cc file
    const std::string Config::kMetadataFilename = "meta.json";
    const std::string Config::kToolpathFilename = "print.jsontoolpath";

	const std::string Config::kLargeThumbnailFilename = "thumbnail_320x200.png";
	const std::string Config::kMediumThumbnailFilename = "thumbnail_110x80.png";
	const std::string Config::kSmallThumbnailFilename = "thumbnail_55x40.png";

	const int Config::kLargeThumbnailWidth = 320;
	const int Config::kMediumThumbnailWidth = 110;
	const int Config::kSmallThumbnailWidth = 55;

	const int Config::kLargeThumbnailHeight = 200;
	const int Config::kMediumThumbnailHeight = 80;
	const int Config::kSmallThumbnailHeight = 40;
}