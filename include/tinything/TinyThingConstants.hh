
#ifndef TINYTHINGCONSTANTS_HH_
#define TINYTHINGCONSTANTS_HH_

#include <string>

namespace LibTinyThing {
    struct TINYTHING_API Config {
        static const std::string kMetadataFilename;
        static const std::string kToolpathFilename;

        static const std::string kLargeThumbnailFilename;
        static const std::string kMediumThumbnailFilename;
        static const std::string kSmallThumbnailFilename;

        static const std::string kFullViewLargeThumbnailFilename;
        static const std::string kFullViewMediumThumbnailFilename;
        static const std::string kFullViewSmallThumbnailFilename;

        static const int kLargeThumbnailWidth;
        static const int kMediumThumbnailWidth;
        static const int kSmallThumbnailWidth;

        static const int kLargeThumbnailHeight;
        static const int kMediumThumbnailHeight;
        static const int kSmallThumbnailHeight;
	};
}

#endif /* TINYTHINGCONSTANTS_HH_ */

