
#include "TinyThingConstants.hh"

namespace LibTinyThing {
    // TODO(pshaw): defining values here to get past linker errors
    // would be ideal to define these in a .cc file
    const std::string Config::kMetadataFilename = "meta.json";
    const std::string Config::kJsonToolpathFilename = "print.jsontoolpath";
    const std::string Config::kGcodeToolpathFilename = "print.gcode";

    const std::string Config::kIsometricLargeThumbnailFilename = "isometric_thumbnail_640x640.png";
    const std::string Config::kIsometricMediumThumbnailFilename = "isometric_thumbnail_320x320.png";
    const std::string Config::kIsometricSmallThumbnailFilename = "isometric_thumbnail_120x120.png";

    const std::string Config::kLargeThumbnailFilename = "thumbnail_320x200.png";
    const std::string Config::kMediumThumbnailFilename = "thumbnail_110x80.png";
    const std::string Config::kSmallThumbnailFilename = "thumbnail_55x40.png";

    const std::string Config::kSombreroLargeThumbnailFilename = "thumbnail_960x1460.png";
    const std::string Config::kSombreroMediumThumbnailFilename = "thumbnail_212x300.png";
    const std::string Config::kSombreroSmallThumbnailFilename = "thumbnail_140x106.png";

    const std::string Config::kSmallSquareThumbnailFilename = "thumbnail_90x90.png";

    const int Config::kIsometricLargeThumbnailWidth = 640;
    const int Config::kIsometricMediumThumbnailWidth = 320;
    const int Config::kIsometricSmallThumbnailWidth = 120;

    const int Config::kIsometricLargeThumbnailHeight = 640;
    const int Config::kIsometricMediumThumbnailHeight = 320;
    const int Config::kIsometricSmallThumbnailHeight = 120;

    const int Config::kLargeThumbnailWidth = 320;
    const int Config::kMediumThumbnailWidth = 110;
    const int Config::kSmallThumbnailWidth = 55;

    const int Config::kLargeThumbnailHeight = 200;
    const int Config::kMediumThumbnailHeight = 80;
    const int Config::kSmallThumbnailHeight = 40;

    const int Config::kSombreroLargeThumbnailWidth = 960;
    const int Config::kSombreroMediumThumbnailWidth = 212;
    const int Config::kSombreroSmallThumbnailWidth = 140;

    const int Config::kSombreroLargeThumbnailHeight = 1460;
    const int Config::kSombreroMediumThumbnailHeight = 300;
    const int Config::kSombreroSmallThumbnailHeight = 106;

    const int Config::kSmallSquareThumbnailWidth = 90;
    const int Config::kSmallSquareThumbnailHeight = 90;
}
